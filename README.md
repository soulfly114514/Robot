# <font color=blue>*快递分拣机器人*</font>

## 一、简介


| 底板材料 | 电机           | 摄像头  |      主控      |
| -------- | -------------- | ------- | :------------: |
| 环氧板   | 42闭环步进电机 | OPENART | STM32F0407VGT6 |


| 开发工具              | 电机           | 摄像头  |      主控      |
| --------------------- | -------------- | ------- | :------------: |
| keil5/vscode EIDE插件 | 42闭环步进电机 | OPENART | STM32F0407VGT6 |
| cubeMX                | 25kg 270°舵机 |         |                |

拓展板BOM 物料清单


| No. | 数量 | Comment                              | Designator        | Footprint                                    | Manufacturer Part                    | Manufacturer               | Supplier Part | Supplier |
| --- | ---- | ------------------------------------ | ----------------- | -------------------------------------------- | ------------------------------------ | -------------------------- | ------------- | -------- |
| 1   | 2    | 4.7nf                                | C8,C29            | C0805                                        | CL21A475KAQNNNE                      | SAMSUNG(三星)              | C1779         | LCSC     |
| 2   | 4    | 220uF                                | C9,C13,C21,C28    | CAP-SMD_BD6.3-L6.6-W6.6-LS7.6-FD-1           | RVE100UF35V67RV0072                  | KNSCHA(科尼盛)             | C2836437      | LCSC     |
| 3   | 2    | 47uf                                 | C10,C27           | C0805                                        | CL21A475KAQNNNE                      | SAMSUNG(三星)              | C1779         | LCSC     |
| 4   | 4    | 100nf                                | C11,C22,C25,C26   | C0805                                        | CL21A475KAQNNNE                      | SAMSUNG(三星)              | C1779         | LCSC     |
| 5   | 2    | 4.7uF                                | C12,C24           | C0805                                        | CL21A475KAQNNNE                      | SAMSUNG(三星)              | C1779         | LCSC     |
| 6   | 2    | 10uF                                 | C14,C15           | CAP-SMD_L3.2-W1.6-RD-C7171                   | TAJA106K016RNJ                       | Kyocera AVX                | C7171         | LCSC     |
| 7   | 2    | 100nF                                | C16,C17           | C0805                                        | CL21A475KAQNNNE                      | SAMSUNG(三星)              | C1779         | LCSC     |
| 8   | 1    | DB2ERC-3.81-2P-GN                    | CN1               | CONN-TH_2P-P3.81_DB2ERC-3.81-2P              | DB2ERC-3.81-2P-GN                    | DORABO(地博电气)           | C395684       | LCSC     |
| 9   | 1    | D2                                   | D2                | SOD-123_L2.7-W1.7-LS3.8-RD                   | BZT52C16                             | TWGMC(台湾迪嘉)            | C727007       | LCSC     |
| 10  | 2    | BSMD0805-010-33V                     | F1,F5             | F0805                                        | BSMD0805-010-33V                     | BHFUSE(佰宏)               | C883101       | LCSC     |
| 11  | 4    | ZX-XH2.54-4PZZ                       | H6,H7,H8,H15      | CONN-TH_4P-P2.50_4PIN                        | ZX-XH2.54-4PZZ                       | Megastar(兆星)             | C7429634      | LCSC     |
| 12  | 4    | Header1*3                            | H9,H10,H11,H12    | HDR-TH_3P-P2.54-V-M                          | PZ254V-11-03P                        | XFCN(兴飞)                 | C2937625      | LCSC     |
| 13  | 2    | 4.7uH                                | L2,L3             | IND-SMD_L5.4-W5.2                            | SLO0530H4R7MTT                       | Sunltech(韩国顺磁)         | C325964       | LCSC     |
| 14  | 3    | FC-2012HRK-620D                      | LED1,LED2,SE1     | LED0805-R-RD                                 | NCD0805R1                            | 国星光电                   | C84256        | LCSC     |
| 15  | 1    | Header-Female-2.54_1x8               | P5                | HDR-TH_8P-P2.54-V-F-1                        | 2.54-1*8P母环保                      | BOOMELE(博穆精密)          | C27438        | LCSC     |
| 16  | 1    | VBE1302                              | Q2                | TO-252-2_L6.6-W6.1-P4.58-LS9.9-TL-1          | VBE1302                              | VBsemi(微碧半导体)         | C480950       | LCSC     |
| 17  | 1    | 4.7K                                 | R18               | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 18  | 1    | 1K                                   | R19               | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 19  | 2    | 24k                                  | R20,R21           | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 20  | 2    | 20kΩ                                | R22,R34           | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 21  | 2    | 200kΩ                               | R23,R35           | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 22  | 1    | 100k                                 | R24               | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 23  | 3    | 10kΩ                                | R25,R32,R36       | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 24  | 1    | 4.7k                                 | R26               | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 25  | 1    | 2.2k                                 | R27               | R0805                                        | 0805W8F2003T5E                       | UNI-ROYAL(厚声)            | C17539        | LCSC     |
| 26  | 1    | 330R                                 | R31               | R0805                                        |                                      |                            |               |          |
| 27  | 1    | 200kΩ                               | R33               | RES-ADJ-TH_3P-L10.0-W10.0-P2.50-L            | 3296W-1-103                          | BOCHEN(博晨)               | C118954       | LCSC     |
| 28  | 1    | SS-12D10L5                           | SW1               | SW-TH_SS-12D10L5                             | SS-12D10L5                           | XKB Connectivity(中国星坤) | C319012       | LCSC     |
| 29  | 1    | LCKFB-LSPI-SkyStar-STM32F407VGT6-PRO | U1                | COMM-TH_LCKFB-LSPI-SKYSTAR-STM32F407VGT6-PRO | LCKFB-LSPI-SkyStar-STM32F407VGT6-PRO | 立创开发板                 | C32710423     | LCSC     |
| 30  | 5    | XH2.54-8                             | U2,U8,U11,U14,U17 | CONN-TH_8P-P2.54_XH2.54_C9900012080          | XH2.54-8                             |                            | C9900012080   | LCSC     |
| 31  | 2    | XT60                                 | U3,U5             | CONN-TH_XT60                                 | XT60                                 | AMASS(艾迈斯)              | C98733        | LCSC     |
| 32  | 2    | SCT2450STER                          | U6,U10            | ESOP-8_L4.9-W3.9-P1.27-LS6.0-BL-EP           | SCT2450STER                          | SCT(芯洲科技)              | C509400       | LCSC     |
| 33  | 1    | RT9013-33GB                          | U7                | SOT-23-5_L3.0-W1.7-P0.95-LS2.8-BL            | RT9013-33GB                          | RICHTEK(立锜)              | C47773        | LCSC     |
| 34  | 1    | GTLP3555                             | U9                | DIP-4_L6.5-W4.6-P2.54-LS7.6-BL               | GTLP3555                             | SUPSiC(国晶微半导体)       | C19271984     | LCSC     |
