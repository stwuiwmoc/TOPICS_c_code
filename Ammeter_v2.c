/**
 * @file Ammeter_v2.c
 * @author Kazuya Nagata
 * @brief
 * @version 2.1
 * @date 2022-12-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "fbiad.h"

void GetCommandLineArgument(int argc_, char **argv_, int *pDnum,
                            int *pChannel_count, int *pCorrection_mode,
                            char **pFile_name);

void ShowHelpText(void);

float CalcVoltageAtAdcBoardInput(float ad_conveted_count_value);

float CalcIna2128Gain(int dnum_, int dnum_bias_, int channel_number);

float CalcVoltageAtBufferBoardOutput(int dnum_, int dnum_bias_,
                                     int channel_number,
                                     float Vxx_adc_input_voltage,
                                     float Vsub_adc_input_voltage_);

float CalcCurrentFromVoltage(float voltage_, float gain_,
                             float current_measurement_resistor_);

float CorrectCurrent(int dnum_, int dnum_bias_, int dnum_clock_,
                     int channel_number, float current);

void GetLocalDatetimeInStr(char **pLocal_datetime);

int main(int argc, char *argv[]) {
    // バイアスライン・クロックラインからそれぞれ繋がっているADCボードの番号を指定
    int dnum_bias = 5;
    int dnum_clock = 4;

    // 電流測定用抵抗の値の指定
    float current_measurement_resistor = 1000;  // [Ω]

    // getopt関数を用いてコマンドライン引数を格納
    int dnum = 5;
    int channel_count = 8;
    int correction_mode = 3;
    char *file_name_current = "bias_current.txt";
    GetCommandLineArgument(argc, argv, &dnum, &channel_count, &correction_mode,
                           &file_name_current);

    // AD変換のみの生データを書き込むファイルのファイル名を作成
    // 例 current.txt から current_raw.txt を作成
    char *suffix = "_raw";
    char *extension = ".txt";

    char file_stem_current[strlen(file_name_current) -
                           strlen(extension)];  // stem は拡張子を除いた部分
    snprintf(file_stem_current,
             strlen(file_name_current) - strlen(extension) + 1, "%s",
             file_name_current);
    char file_name_raw[strlen(file_name_current) + strlen(extension)];
    sprintf(file_name_raw, "%s%s%s", file_stem_current, suffix, extension);

    // dnum に応じて表示するチャンネル名のラベルを書き換え
    char channel_name[8][9];
    if (dnum == dnum_bias) {
        char channel_name_bias[8][9] = {"V3      ", "AGND    ", "Vdet    ",
                                        "Vdetgate", "Vddout  ", "Vdduc   ",
                                        "Vgg     ", "Vsub    "};

        for (int i = 0; i < channel_count; i++) {
            strcpy(channel_name[i], channel_name_bias[i]);
        }
    } else if (dnum == dnum_clock) {
        char channel_name_clock[8][9] = {"syncS   ", "1S      ", "2S      ",
                                         "syncF   ", "1F      ", "2F      ",
                                         "rst     ", "N.C.    "};

        for (int i = 0; i < channel_count; i++) {
            strcpy(channel_name[i], channel_name_clock[i]);
        }
    } else {
        for (int i = 0; i < channel_count; i++) {
            strcpy(channel_name[i], "N.C.    ");
        }
    }

    // 保存用ファイルの新規行に、入力されたコマンドライン引数を書き込み
    FILE *fp_current;
    fp_current = fopen(file_name_current, "a+");
    if (fp_current == NULL) {
        printf("Error: file cannot open\n");
        exit(1);
    } else {
        fprintf(fp_current, "%s ", argv[0]);
        fprintf(fp_current, "-d %d ", dnum);
        fprintf(fp_current, "-n %d ", channel_count);
        fprintf(fp_current, "-c %d ", correction_mode);
        fprintf(fp_current, "-f %s ", file_name_current);
        fprintf(fp_current, "\n");
    }
    fclose(fp_current);

    FILE *fp_raw;
    fp_raw = fopen(file_name_raw, "a+");
    if (fp_raw == NULL) {
        printf("Error: file cannot open\n");
        exit(1);
    } else {
        fprintf(fp_raw, "%s ", argv[0]);
        fprintf(fp_raw, "-d %d ", dnum);
        fprintf(fp_raw, "-n %d ", channel_count);
        fprintf(fp_raw, "-c %d ", correction_mode);
        fprintf(fp_raw, "-f %s ", file_name_current);
        fprintf(fp_raw, "\n");
    }
    fclose(fp_raw);

    // ADCボードへのIOポートを開く
    int nRet;
    ADSMPLREQ AdSmplConfig;
    unsigned short sample_data[1024][channel_count];

    nRet = AdOpen(dnum);
    if (nRet != AD_ERROR_SUCCESS) {
        printf("Error: Current ADC Board Open Error\nAlready measuring?");
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
        AdGetStatus(dnum, &ul_ad_sample_status, &ul_ad_sample_count,
                    &ul_ad_available_count);

        // データの取得
        AdGetSamplingData(dnum, sample_data, &ul_ad_sample_count);

        // 保存用ファイルのopen
        FILE *fp_current;
        fp_current = fopen(file_name_current, "a+");
        if (fp_current == NULL) {
            printf("cannot open\n");
            exit(1);
        }

        FILE *fp_raw;
        fp_raw = fopen(file_name_raw, "a+");
        if (fp_raw == NULL) {
            printf("cannot open\n");
            exit(1);
        }

        // 補正モードを表示
        printf("-c = %d\n", correction_mode);

        // 保存した時刻を書き込み
        char *local_time = NULL;
        GetLocalDatetimeInStr(&local_time);
        fprintf(fp_current, "%s ", local_time);
        fprintf(fp_raw, "%s ", local_time);

        // mallocで動的にメモリを確保したので不要になったら開放
        free(local_time);

        // チャンネル毎に処理する
        for (int k = 0; k < channel_count; k++) {
            // INA2128 のゲイン計算（Vdducラインのみゲインが異なる）
            float gain = CalcIna2128Gain(dnum, dnum_bias, k);

            // 電流の平均値計算用の変数の初期化
            float current_sum = 0;
            float current_average = 0;

            // 一つのチャンネルに格納されたデータの処理
            for (unsigned long j = 0; j < ul_ad_sample_count; j++) {
                // AD変換された読み出し値をアナログ電圧値に換算し直す
                float ad_converted_count_value = sample_data[j][k];
                float adc_input_voltage =
                    CalcVoltageAtAdcBoardInput(ad_converted_count_value);

                float buffer_board_output_voltage;
                if (correction_mode == 1 || correction_mode == 3) {
                    // バッファボードのAGNDずれの補正を行う場合

                    // Vsubラインのアナログ電圧値を計算（補正で必要）
                    float Vsub_ad_converted_count_value = sample_data[j][7];
                    float Vsub_adc_input_voltage = CalcVoltageAtAdcBoardInput(
                        Vsub_ad_converted_count_value);

                    // バイアスバッファボードの補正を行う
                    buffer_board_output_voltage =
                        CalcVoltageAtBufferBoardOutput(dnum, dnum_bias, k,
                                                       adc_input_voltage,
                                                       Vsub_adc_input_voltage);

                } else {
                    buffer_board_output_voltage = adc_input_voltage;
                }

                // 電圧値を電流値に直す
                float current_data =
                    CalcCurrentFromVoltage(buffer_board_output_voltage, gain,
                                           current_measurement_resistor);

                if (correction_mode == 2 || correction_mode == 3) {
                    // KENWOOD DL-97での校正結果をもとに電流値の補正を行う場合
                    current_data = CorrectCurrent(dnum, dnum_bias, dnum_clock,
                                                  k, current_data);
                }

                // 電流値を格納されたデータ分で平均する
                current_sum += current_data;
                current_average = current_sum / ul_ad_sample_count;

                // 格納されている分を全て読み出し終わったら電流値を表示・保存する
                if (j == ul_ad_sample_count - 1) {
                    // 標準出力への表示
                    printf("%s ", channel_name[k]);
                    printf("(uA) = %.4f\n", current_average);

                    // 保存用ファイルへの書き込み
                    fprintf(fp_current, "%f ", current_average);
                    fprintf(fp_raw, "%u ", sample_data[0][k]);
                }
            }
        }

        // 全チャンネル書き込み終わったら改行
        fprintf(fp_current, "\n");
        fprintf(fp_raw, "\n");

        // 保存用ファイルの close
        fclose(fp_current);
        fclose(fp_raw);

        printf("\n");
    }
    nRet = AdClose(dnum);

    // mallocで動的にメモリを確保しているので、終了前にメモリを開放する
    free(file_name_current);
    return 0;
}

/**
 * @brief コマンドライン引数を3つのアドレスに格納する
 *
 * @param argc_ コマンドライン引数のargc
 * @param argv_ コマンドライン引数のargv
 * @param pDnum ポインタ（引数 "-d" の値が格納される）
 * @param pChannel_count ポインタ（引数 "-n" の値が格納される）
 * @param pCorrection_mode ポインタ（引数 "-c" の値が格納される）
 * @param pFile_name ポインタ（引数 "-f" の文字列が格納される）
 */
void GetCommandLineArgument(int argc_, char **argv_, int *pDnum,
                            int *pChannel_count, int *pCorrection_mode,
                            char **pFile_name) {
    int ret = -1;

    while (1) {
        // getopt関数によってグローバル変数 optarg が定義される
        ret = getopt(argc_, argv_, "hd:n:c:f:");

        if (ret == -1) break;

        switch (ret) {
            case 'h':
                // show help text
                ShowHelpText();
                exit(0);
                break;

            case 'd':
                *pDnum = atoi(optarg);
                break;

            case 'n':
                *pChannel_count = atoi(optarg);
                break;

            case 'c':
                *pCorrection_mode = atoi(optarg);
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

void ShowHelpText(void) {
    printf("\n");
    printf("=============== Usage ===============\n");
    printf("\n");
    printf("'-h' ヘルプ\n");
    printf("    ヘルプを表示し、プログラムを終了します。\n");
    printf("\n");
    printf("'-d' デバイス識別番号\n");
    printf("    測定に使用するADCボードの識別用番号を以下から選んで入力してください。\n");
    printf("        4 : clock lines [syncS, 1S, 2S, syncF, 1F, 2F, rst]\n");
    printf("        5 : bias lines [V3, AGND, Vdet, Vdetgate, Vddout, Vdduc, Vgg, Vsub]\n");
    printf("    この番号はLPC-321416 / LPC-321216 の物理ロータリースイッチ RSW1 の番号と対応しています。\n");
    printf("    使用するADCボードや RSW1 の番号を変更する場合には、\n");
    printf("    本プログラムのソースコード中の変数 dnum_bias dnum_clock の値も合わせて変更してください。\n");
    printf("\n");
    printf("'-n' 測定チャンネル数\n");
    printf("    測定を行うチャンネルの数を 1 ~ 8 の整数値で入力してください。\n");
    printf("\n");
    printf("'-c' 補正モード\n");
    printf("    表示・保存する電流値に対して各種補正の有無を、以下から選んで入力してください。\n");
    printf("        0 : 補正を一切行わずに出力します\n");
    printf("        1 : バイアスバッファボードから出力される電圧ラインに対して、AGNDずれの補正を行います。\n");
    printf("        2 : クロックバッファボードから出力される電圧ラインに対して、校正結果に近づける補正を行います。\n");
    printf("        3 : 1 と 2 の補正を両方行います。\n");
    printf("    これらの補正内容の詳細については、Nagata (2022) を参照してください。\n");
    printf("    補正の根拠となった校正結果については、ソースコード中の該当関数のコメントアウトに記載されたリンク先を参照してください。\n");
    printf("\n");
    printf("'-f' 保存先ファイルパス\n");
    printf("    保存先のファイルパスを入力してください。\n");
    printf("    拡張子はプレーンテキスト '.txt' にのみ対応しています。\n");
    printf("    パスに対応したファイルが自動作成され、'-c' で選択した補正が行われた電流値が保存されます。\n");
    printf("    また、パスの拡張子の直前に '_raw' を追加したファイルが自動作成され、AD変換まで済んだ状態の生データが保存されます。\n");
    printf("\n");
}

/**
 * @brief
 * AD変換された読み出しデータをADCボード入力時点でのアナログ電圧値[V]に換算し直す
 *
 * @param ad_conveted_count_value_ float AD変換された読み出し値
 * @return float [V] ADCボード入力時点でのアナログ電圧値
 */
float CalcVoltageAtAdcBoardInput(float ad_conveted_count_value_) {
    float adc_resolution = pow(2.0, 16.0);

    float input_min_voltage = -10.0;  // [V]
    float input_max_voltage = +10.0;  // [V]

    float voltage_range = input_max_voltage - input_min_voltage;

    float input_voltage =
        ad_conveted_count_value_ * (voltage_range / adc_resolution) +
        input_min_voltage;

    return input_voltage;
}

/**
 * @brief 各バイアスライン・クロックラインでのINA2128の差動増幅ゲインを計算する
 *
 * @param dnum_ コマンドライン引数 "-d" の値（ADCボードの番号）
 * @param dnum_bias_ バイアスラインを計測しているADCボードの番号
 * @param channel_number ゲインを計算したいチャンネルの番号
 * @return float チャンネルに対応したINA2128のゲイン
 */
float CalcIna2128Gain(int dnum_, int dnum_bias_, int channel_number) {
    float gain_;
    if (dnum_ == dnum_bias_ && channel_number == 5) {
        gain_ = 1;
    } else {
        float gain_resistor = 2700.0;  //[Ω]

        // 計算式はINA2128のデータシートを参照
        gain_ = 1.0 + 50000.0 / gain_resistor;
    }
    return gain_;
}

/**
 * @brief Vsubラインを用いてバイアスバッファボードのAGNDずれを補正する
 *
 * @param dnum_ 測定中のADCボードの番号
 * @param dnum_bias_ バイアスラインを測定するADCボードの番号
 * @param channel_number 測定中のチャンネルの番号
 * @param Vxx_adc_input_voltage 各電圧ラインのadcボードの入力電圧
 * @param Vsub_adc_input_voltage_ Vsubラインのadcボードの入力電圧
 * @return float AGNDずれが補正された電圧
 */
float CalcVoltageAtBufferBoardOutput(int dnum_, int dnum_bias_,
                                     int channel_number,
                                     float Vxx_adc_input_voltage,
                                     float Vsub_adc_input_voltage_) {
    float output_voltage;
    if (dnum_ == dnum_bias_) {
        // バイアスラインの場合
        if (channel_number == 0 || channel_number == 1) {
            // V3 または AGND ラインは
            // クロックバッファボードからの出力のためAGNDずれは発生せず、補正は不要
            output_voltage = Vxx_adc_input_voltage;
        } else {
            // バイアスラインの V3 と AGND 以外の電圧ラインは
            // バイアスバッファボードからの出力のためAGNDずれが発生、補正が必要
            output_voltage = Vxx_adc_input_voltage - Vsub_adc_input_voltage_;
        }
    } else {
        // クロックラインの場合
        // 全てクロックバッファボードからの出力のためAGNDずれは発生せず、補正は不要
        output_voltage = Vxx_adc_input_voltage;
    }
    return output_voltage;
}

/**
 * @brief INA2128で差動増幅された電圧値から各電圧ラインで流れる電流を計算
 *
 * @param voltage_ [V] INA2128で差動増幅が終わった段階の電圧値
 * @param gain_ INA2128のゲイン
 * @param current_measurement_resistor_ [Ω] 電流測定用抵抗の値
 * @return float [uA] 各電圧ラインで流れる電流値
 */
float CalcCurrentFromVoltage(float voltage_, float gain_,
                             float current_measurement_resistor_) {
    // 差動増幅前の値、つまり電流測定用抵抗出の電圧降下を計算
    float voltage_drop = voltage_ / gain_;  // [V]

    // 電圧降下から流れる電流を計算
    float current_A = voltage_drop / current_measurement_resistor_;  // [A]

    // [uA] に単位換算
    float current_uA = current_A * pow(10, 6);
    return current_uA;
}

/**
 * @brief KENWOOD DL-97での測定結果から得られた係数を使って電流値を補正
 *
 * @param dnum_ 測定中のADCボードの番号
 * @param dnum_bias_ バイアスラインを測定するADCボードの番号
 * @param dnum_clock_ クロックラインを測定するADCボードの番号
 * @param channel_number 測定中のチャンネルの番号
 * @param current 補正前の電流値
 * @return float 補正後の電流値
 */
float CorrectCurrent(int dnum_, int dnum_bias_, int dnum_clock_,
                     int channel_number, float current) {
    // https://drive.google.com/drive/folders/188J55OwmpbHIqxtHhpQhCIOJteF3k_fJ?usp=share_link
    // 上のリンク先の各電圧ラインの回帰直線の傾きを a 、切片を b とする
    // 補正前の電流値を X 、補正後の電流値を Y として Y = aX + b として補正を行う。

    float corrected_current;

    if (dnum_ == dnum_clock_) {
        // クロックラインの場合

        float a_clock_lines[7] = {
            0.988,  // syncS
            0.993,  // 1S
            1.023,  // 2S
            1.007,  // syncF
            1.019,  // 1F
            1.019,  // 2F
            1.006   // rst
        };
        float b_clock_lines[7] = {
            0.422,  // syncS
            0.430,  // 1S
            0.285,  // 2S
            0.251,  // syncF
            0.256,  // 1F
            0.256,  // 2F
            0.276   // rst
        };
        float a = a_clock_lines[channel_number];
        float b = b_clock_lines[channel_number];
        corrected_current = a * current + b;

    } else if (dnum_ == dnum_bias_ && channel_number == 0) {
        // V3 ラインの場合
        float a = 0.988;
        float b = -0.096;
        corrected_current = a * current + b;
    } else {
        // それ以外の電圧ラインでは補正は行わない
        corrected_current = current;
    }
    return corrected_current;
}

/**
 * @brief 現在日時を文字列として取得する
 *
 * @param pLocal_datetime ポインタ（文字列が保存される）
 */
void GetLocalDatetimeInStr(char **pLocal_datetime) {
    // 現在日時を文字列として取得 参考
    // https://www.ibm.com/docs/ja/zos/2.4.0?topic=functions-strftime-convert-formatted-time
    char local_datetime[100];
    time_t temp;
    struct tm *timeptr;

    temp = time(NULL);
    timeptr = localtime(&temp);
    strftime(local_datetime, sizeof(local_datetime) - 1, "%x-%X", timeptr);

    // 以下は文字列を引数として返すための処理 参考 https://skpme.com/211/
    char *cp = NULL;
    cp = (char *)malloc(sizeof(char) * 100);
    if (cp == NULL) {
        printf("メモリ不足で文字列配列の作成失敗\n");
    }
    strcpy(cp, local_datetime);
    *pLocal_datetime = cp;
}
