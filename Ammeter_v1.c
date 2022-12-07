#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fbiad.h"

int main(int argc, char *argv[]) {
    int dnum;
    int dnum_bias = 5;
    int dnum_clock = 4;

    int channel_count;
    char file_name[100];

    /***********************************/
    /*** 0, Interpret the arguments  ***/
    /***********************************/
    while ((argc > 1) && (argv[1][0] == '-')) {
        switch (argv[1][1]) {
            case 'd':
                if (argv[1][2] == '\0') {
                    --argc;
                    ++argv;
                    dnum = atoi(argv[1]);
                } else
                    dnum = atoi(&argv[1][2]);
                break;
            case 'n':
                if (argv[1][2] == '\0') {
                    --argc;
                    ++argv;
                    channel_count = atoi(argv[1]);
                } else
                    channel_count = atoi(&argv[1][2]);
                break;
            case 'f':
                if (argv[1][2] == '\0') {
                    --argc;
                    ++argv;
                    strcpy(file_name, argv[1]);
                } else
                    strcpy(file_name, "current.txt");
                break;
            default:
                fprintf(stderr, "Bad option: %s\n", argv[1]);
        }
        --argc;  // decriment the count
        ++argv;  // move to next option
    }

    printf("\n");
    printf("Device=");
    printf("%01X ", dnum);
    printf("\n");

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

    unsigned long i;
    AdSmplConfig.ulChCount = channel_count;

    for (i = 0; i < channel_count; i++) {
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

        unsigned long k;
        unsigned long j;
        float adc_resolution = pow(2.0, 16.0);
        float gain_resistor = 2700.0;
        float gain = 1.0 + 50000.0 / gain_resistor;
        float current_data[1024][channel_count];
        float sum[channel_count];
        float ave[channel_count];

        char channel_name[8][9];

        if (dnum == dnum_bias) {
            char channel_name_bias[8][9] = {
                "V3      ", "AGND    ", "Vdet    ", "Vdetgate",
                "Vddout  ", "Vdduc   ", "Vgg     ", "Vsub    "};

            for (i = 0; i < 8; i++) {
                strcpy(channel_name[i], channel_name_bias[i]);
            }
        } else if (dnum == dnum_clock) {
            char channel_name_clock[8][9] = {
                "syncS   ", "1S      ", "2S      ", "syncF   ",
                "1F      ", "2F      ", "rst     ", "N.C.    "};

            for (i = 0; i < 8; i++) {
                strcpy(channel_name[i], channel_name_clock[i]);
            }
        } else {
            for (i = 0; i < 8; i++) {
                strcpy(channel_name[i], "N.C.    ");
            }
        }

        int l;
        for (l = 0; l < channel_count; l++) {
            sum[l] = 0.0;
            ave[l] = 0.0;
        }

        for (j = 0; j < ul_ad_sample_count; j++) {
            // file open
            FILE *fp;
            fp = fopen(file_name, "a+");
            if (fp == NULL) {
                printf("cannot open\n");
                exit(1);
            }

            for (k = 0; k < channel_count; k++) {
                // data print
                float voltage_data = sample_data[j][k] * 20.0 / adc_resolution - 10.0;

                if (dnum == dnum_bias && k == 5) {
                    current_data[j][k] = voltage_data / 1000.0 * pow(10, 6);
                } else {
                    current_data[j][k] = (voltage_data / gain) / 1000.0 * pow(10, 6);
                }

                sum[k] += current_data[j][k];
                ave[k] = sum[k] / ul_ad_sample_count;
            }

            if (j == ul_ad_sample_count - 1) {
                int m;
                for (m = 0; m < channel_count; m++) {
                    printf("%s ", channel_name[m]);
                    printf("(uA) = %.4f", ave[m]);
                    printf("\n");
                    // save data
                    fprintf(fp, "%f ", ave[m]);
                }
                fprintf(fp, "\n");
                printf("\n");
            }
            fclose(fp);
        }
        printf("\n");
    }
    nRet = AdClose(dnum);
    return 0;
}
