#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fbiad.h"

int main(int argc, char *argv[]) {
    int dnum;
    int dnum_bias = 5;
    int dnum_clock = 4;

    int ChCount;
    char flname[100];

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
                    ChCount = atoi(argv[1]);
                } else
                    ChCount = atoi(&argv[1][2]);
                break;
            case 'f':
                if (argv[1][2] == '\0') {
                    --argc;
                    ++argv;
                    strcpy(flname, argv[1]);
                } else
                    strcpy(flname, "current.txt");
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
    // unsigned long ulSmplNum;
    // unsigned long ulSmplEventNum;
    unsigned short SmplData[1024][ChCount];

    nRet = AdOpen(dnum);
    if (nRet != AD_ERROR_SUCCESS) {
        printf("Open Error\n");
        return -1;
    }

    // サンプリング条件の取得
    AdGetSamplingConfig(dnum, &AdSmplConfig);

    unsigned long i;
    AdSmplConfig.ulChCount = ChCount;

    for (i = 0; i < ChCount; i++) {
        AdSmplConfig.SmplChReq[i].ulChNo = i + 1;
        AdSmplConfig.SmplChReq[i].ulRange = AD_10V;
    }
    AdSmplConfig.ulSingleDiff = AD_INPUT_SINGLE;
    // AdSmplConfig.ulSmplNum = 1024;
    // AdSmplConfig.ulSmplEventNum = 256;
    // AdSmplConfig.ulTrigMode = AD_ETERNITY;

    // サンプリング条件の設定
    AdSetSamplingConfig(dnum, &AdSmplConfig);

    while (1) {
        // 連続サンプリングの開始
        AdStartSampling(dnum, FLAG_SYNC);

        unsigned long ulAdSmplStatus;
        unsigned long ulAdSmplCount;
        unsigned long ulAdAvailCount;
        AdGetStatus(dnum, &ulAdSmplStatus, &ulAdSmplCount, &ulAdAvailCount);
        // printf("Count:%ld\n", ulAdSmplCount);

        // データの取得
        // ulSmplNum = 1024;
        AdGetSamplingData(dnum, SmplData, &ulAdSmplCount);
        int ulSmpl = ulAdSmplCount;
        // printf("Count:%ld\n", ulAdSmplCount);

        unsigned long k;
        unsigned long j;
        float al = pow(2.0, 16.0);
        float resi = 2700.0;
        float gain = 1.0 + 50000.0 / resi;
        float Cdata[1024][ChCount];
        float sum[ChCount];
        float ave[ChCount];

        char chname[8][9];

        if (dnum == dnum_bias) {
            char chname_bias[8][9] = {
                "V3      ", "AGND    ", "Vdet    ", "Vdetgate",
                "Vddout  ", "Vdduc   ", "Vgg     ", "Vsub    "};

            for (i = 0; i < 8; i++) {
                strcpy(chname[i], chname_bias[i]);
            }
        } else if (dnum == dnum_clock) {
            char chname_clock[8][9] = {
                "syncS   ", "1S      ", "2S      ", "syncF   ",
                "1F      ", "2F      ", "rst     ", "N.C.    "};

            for (i = 0; i < 8; i++) {
                strcpy(chname[i], chname_clock[i]);
            }
        } else {
            for (i = 0; i < 8; i++) {
                strcpy(chname[i], "N.C.    ");
            }
        }

        int l;
        for (l = 0; l < ChCount; l++) {
            sum[l] = 0.0;
            ave[l] = 0.0;
        }

        for (j = 0; j < ulSmpl; j++) {
            // file open
            FILE *fp;
            fp = fopen(flname, "a+");
            if (fp == NULL) {
                printf("cannot open\n");
                exit(1);
            }
            // printf("%d ", ChCount);
            for (k = 0; k < ChCount; k++) {
                // data print
                float Adata = SmplData[j][k] * 20.0 / al - 10.0;

                if (dnum == dnum_bias && k == 5) {
                    Cdata[j][k] = Adata / 1000.0 * pow(10, 6);
                } else {
                    Cdata[j][k] = (Adata / gain) / 1000.0 * pow(10, 6);
                }

                sum[k] += Cdata[j][k];
                // printf("%f ", sum[k]);
                ave[k] = sum[k] / ulSmpl;
            }

            if (j == ulSmpl - 1) {
                int m;
                for (m = 0; m < ChCount; m++) {
                    printf("%s ", chname[m]);
                    // printf("Ch%d_", m+1);
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
