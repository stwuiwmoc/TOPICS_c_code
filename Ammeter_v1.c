/**
 * @file Ammeter_v1.c
 * @author Kazuya Nagata
 * @brief
 * @version 1.0
 * @date 2022-12-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fbiad.h"

void GetCommandLineArgument(int argc_, char **argv_, int *pDnum,
                            int *pChannel_count, char **pFile_name);

float CalcVoltageAtAdcBoardInput(float ad_conveted_count_value);

int main(int argc, char *argv[]) {
    int dnum_bias = 5;
    int dnum_clock = 4;

    int dnum;
    int channel_count;
    char *file_name = "current.txt";

    // getopt関数を用いてコマンドライン引数を格納
    GetCommandLineArgument(argc, argv, &dnum, &channel_count, &file_name);

    // dnum に応じて表示するチャンネル名のラベルを書き換え
    char channel_name[8][9];

    if (dnum == dnum_bias) {
        char channel_name_bias[8][9] = {
            "V3      ", "AGND    ", "Vdet    ", "Vdetgate",
            "Vddout  ", "Vdduc   ", "Vgg     ", "Vsub    "};

        for (int i = 0; i < channel_count; i++) {
            strcpy(channel_name[i], channel_name_bias[i]);
        }
    } else if (dnum == dnum_clock) {
        char channel_name_clock[8][9] = {
            "syncS   ", "1S      ", "2S      ", "syncF   ",
            "1F      ", "2F      ", "rst     ", "N.C.    "};

        for (int i = 0; i < channel_count; i++) {
            strcpy(channel_name[i], channel_name_clock[i]);
        }
    } else {
        for (int i = 0; i < channel_count; i++) {
            strcpy(channel_name[i], "N.C.    ");
        }
    }

    int nRet;
    ADSMPLREQ AdSmplConfig;
    unsigned short sample_data[1024][channel_count];

    nRet = AdOpen(dnum);
    if (nRet != AD_ERROR_SUCCESS) {
        printf("Open Error\n");
        return -1;
    }

    // サンプリング条件の取得
    AdGetSamplingConfig(dnum, &AdSmplConfig);

    AdSmplConfig.ulChCount = channel_count;

    for (unsigned long i = 0; i < channel_count; i++) {
        AdSmplConfig.SmplChReq[i].ulChNo = i + 1;
        AdSmplConfig.SmplChReq[i].ulRange = AD_10V;
    }
    AdSmplConfig.ulSingleDiff = AD_INPUT_SINGLE;

    // サンプリング条件の設定
    AdSetSamplingConfig(dnum, &AdSmplConfig);

    while (1) {
        // 連続サンプリングの開始
        AdStartSampling(dnum, FLAG_SYNC);

        unsigned long ul_ad_sample_status;
        unsigned long ul_ad_sample_count;
        unsigned long ul_ad_available_count;
        AdGetStatus(dnum, &ul_ad_sample_status, &ul_ad_sample_count, &ul_ad_available_count);

        // データの取得
        AdGetSamplingData(dnum, sample_data, &ul_ad_sample_count);

        // file open
        FILE *fp;
        fp = fopen(file_name, "a+");
        if (fp == NULL) {
            printf("cannot open\n");
            exit(1);
        }

        float sum[channel_count];
        float ave[channel_count];

        for (int i = 0; i < channel_count; i++) {
            sum[i] = 0.0;
            ave[i] = 0.0;
        }

        for (int k = 0; k < channel_count; k++) {

            float gain;
            if (dnum == dnum_bias && k == 5) {
                gain = 1;
            } else {
                float gain_resistor = 2700.0;
                gain = 1.0 + 50000.0 / gain_resistor;
            }

            for (unsigned long j = 0; j < ul_ad_sample_count; j++) {

                // AD変換された読み出し値をアナログ電圧値に換算し直す
                float ad_converted_count_value = sample_data[j][k];
                float adc_input_voltage = CalcVoltageAtAdcBoardInput(ad_converted_count_value);

                // 電圧値を電流値に直す
                float current_data = (adc_input_voltage / gain) / 1000.0 * pow(10, 6);

                // 電流値を平均する
                sum[k] += current_data;
                ave[k] = sum[k] / ul_ad_sample_count;

                // 格納されている分を全て読み出し終わったら電流値を表示・保存する
                if (j == ul_ad_sample_count - 1) {
                    // 標準出力への表示
                    printf("%s ", channel_name[k]);
                    printf("(uA) = %.4f\n", ave[k]);

                    // ファイルへの保存
                    fprintf(fp, "%f ", ave[k]);
                }
            }
        }
        fprintf(fp, "\n");

        // file close
        fclose(fp);

        printf("\n");
    }
    nRet = AdClose(dnum);
    free(file_name); // file_name はmallocで動的にメモリを確保しているので、終了前にメモリを開放する
    return 0;
}

/**
 * @brief コマンドライン引数を3つのアドレスに格納する
 *
 * @param argc_ コマンドライン引数のargc
 * @param argv_ コマンドライン引数のargv
 * @param pDnum ポインタ（引数 "-d" の値が格納される）
 * @param pChannel_count ポインタ（引数 "-n" の値が格納される）
 * @param pFile_name ポインタ（引数 "-f" の文字列が格納される）
 */
void GetCommandLineArgument(int argc_, char **argv_, int *pDnum,
                            int *pChannel_count, char **pFile_name) {
    int ret = -1;

    while (1) {
        // getopt関数によってグローバル変数 optarg が定義される
        ret = getopt(argc_, argv_, "hd:n:f:");

        if (ret == -1) break;

        switch (ret) {
            case 'h':
                // show help text
                printf("\nhelp text will be added\n\n");
                exit(0);
                break;

            case 'd':
                *pDnum = atoi(optarg);
                break;

            case 'n':
                *pChannel_count = atoi(optarg);
                break;

            case 'f': {
                // 以下は複数の戻り値（文字列含む）を返すための処理 参考
                // https://skpme.com/211/
                char *cp = NULL;
                cp = (char *)malloc(sizeof(char) * 100);
                if (cp == NULL) {
                    printf("メモリ不足で文字列配列の作成失敗\n");
                }
                strcpy(cp, optarg);
                *pFile_name = cp;
            } break;

            case '?':
                printf("???\n");
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", ret);
        }
    }
}

/**
 * @brief AD変換された読み出しデータをADCボード入力時点でのアナログ電圧値[V]に換算し直す
 *
 * @param ad_conveted_count_value_ float AD変換された読み出し値
 * @return float [V] ADCボード入力時点でのアナログ電圧値
 */
float CalcVoltageAtAdcBoardInput(float ad_conveted_count_value_) {
    float adc_resolution = pow(2.0, 16.0);

    float input_min_voltage = -10.0; // [V]
    float input_max_voltage = +10.0; // [V]

    float input_voltage_range = input_max_voltage - input_min_voltage;

    float input_voltage = ad_conveted_count_value_ * (input_voltage_range / adc_resolution) + input_min_voltage;

    return input_voltage;
}
