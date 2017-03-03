/*
 * path_mega128.h
 *
 * Created: 31.08.2016 15:48:45
 *  Author: GumerovMN
 */ 


#ifndef PATH_MEGA128_H_
#define PATH_MEGA128_H_

/* 0x3F SREG */
#define SREG      _SFR_IO8(0x3F)

/* 2-wire Control Register - TWCR */
#define    TWCR_TWINT	_portx(TWCR).bit7
#define    TWCR_TWEA	_portx(TWCR).bit6
#define    TWCR_TWSTA	_portx(TWCR).bit5
#define    TWCR_TWSTO	_portx(TWCR).bit4
#define    TWCR_TWWC	_portx(TWCR).bit3
#define    TWCR_TWEN	_portx(TWCR).bit2
#define    TWCR_TWIE	_portx(TWCR).bit0

/* 2-wire Address Register - TWAR */
#define    TWAR_TWA6	_portx(TWAR).bit7
#define    TWAR_TWA5	_portx(TWAR).bit6
#define    TWAR_TWA4	_portx(TWAR).bit5
#define    TWAR_TWA3	_portx(TWAR).bit4
#define    TWAR_TWA2	_portx(TWAR).bit3
#define    TWAR_TWA1	_portx(TWAR).bit2
#define    TWAR_TWA0	_portx(TWAR).bit1
#define    TWAR_TWGCE	_portx(TWAR).bit0

/* 2-wire Status Register - TWSR */
#define    TWSR_TWS7	_portx(TWSR).bit7
#define    TWSR_TWS6	_portx(TWSR).bit6
#define    TWSR_TWS5	_portx(TWSR).bit5
#define    TWSR_TWS4	_portx(TWSR).bit4
#define    TWSR_TWS3	_portx(TWSR).bit3
#define    TWSR_TWPS1	_portx(TWSR).bit1
#define    TWSR_TWPS0	_portx(TWSR).bit0

/* External Memory Control Register A - XMCRA */
typedef struct
{
	unsigned bit0:1;
	unsigned bit1:1;
	unsigned bit2:1;
	unsigned bit3:1;
	unsigned bit4:1;
	unsigned bit5:1;
	unsigned bit6:1;
	unsigned bit7:1;
} BYTE_XMCRA;
#define    SRL2         6
#define    SRL1         5
#define    SRL0         4
#define    SRW01        3
#define    SRW00        2
#define    SRW11        1

/* External Memory Control Register B - XMCRA */
#define    XMBK         7
#define    XMM2         2
#define    XMM1         1
#define    XMM0         0

/* XDIV Divide control register - XDIV */
#define    XDIVEN       7
#define    XDIV6        6
#define    XDIV5        5
#define    XDIV4        4
#define    XDIV3        3
#define    XDIV2        2
#define    XDIV1        1
#define    XDIV0        0

/* RAM Page Z select register - RAMPZ */
#define     RAMPZ0      0

/* External Interrupt Control Register A - EICRA */
#define    ISC31        7
#define    ISC30        6
#define    ISC21        5
#define    ISC20        4
#define    ISC11        3
#define    ISC10        2
#define    ISC01        1
#define    ISC00        0

/* External Interrupt Control Register B - EICRB */
#define    ISC71        7
#define    ISC70        6
#define    ISC61        5
#define    ISC60        4
#define    ISC51        3
#define    ISC50        2
#define    ISC41        1
#define    ISC40        0

/* Store Program Memory Control Register - SPMCSR, SPMCR */
#define    SPMIE        7
#define    RWWSB        6
#define    RWWSRE       4
#define    BLBSET       3
#define    PGWRT        2
#define    PGERS        1
#define    SPMEN        0

/* External Interrupt MaSK register - EIMSK */
#define    INT7         7
#define    INT6         6
#define    INT5         5
#define    INT4         4
#define    INT3         3
#define    INT2         2
#define    INT1         1
#define    INT0         0

/* External Interrupt Flag Register - EIFR */
#define    INTF7        7
#define    INTF6        6
#define    INTF5        5
#define    INTF4        4
#define    INTF3        3
#define    INTF2        2
#define    INTF1        1
#define    INTF0        0

/* Timer/Counter Interrupt MaSK register - TIMSK */
#define TIMSK_OCIE2		__port(TIMSK).bit7
#define TIMSK_TOIE2		__port(TIMSK).bit6
#define TIMSK_TICIE1	__port(TIMSK).bit5
#define TIMSK_OCIE1A	__port(TIMSK).bit4
#define TIMSK_OCIE1B	__port(TIMSK).bit3
#define TIMSK_TOIE1		__port(TIMSK).bit2
#define TIMSK_OCIE0		__port(TIMSK).bit1
#define TIMSK_TOIE0		__port(TIMSK).bit0

/* Timer/Counter Interrupt Flag Register - TIFR */
#define    TIFR_OCF2    __port(TIFR).bit7
#define    TIFR_TOV2    __port(TIFR).bit6
#define    TIFR_ICF1    __port(TIFR).bit5
#define    TIFR_OCF1A   __port(TIFR).bit4
#define    TIFR_OCF1B   __port(TIFR).bit3
#define    TIFR_TOV1    __port(TIFR).bit2
#define    TIFR_OCF0    __port(TIFR).bit1
#define    TIFR_TOV0	__port(TIFR).bit0

/* Extended Timer Interrupt MaSK register - ETIMSK */
#define    TICIE3       5
#define    OCIE3A       4
#define    OCIE3B       3
#define    TOIE3        2
#define    OCIE3C       1
#define    OCIE1C       0

/* Extended Timer Interrupt Flag Register - ETIFR */
#define    ICF3         5
#define    OCF3A        4
#define    OCF3B        3
#define    TOV3         2
#define    OCF3C        1
#define    OCF1C        0

/* MCU general Control Register - MCUCR */
#define    SRE          7
#define    SRW          6
#define    SRW10        6       /* new name in datasheet (2467E-AVR-05/02) */
#define    SE           5
#define    SM1          4
#define    SM0          3
#define    SM2          2
#define    IVSEL        1
#define    IVCE         0

/* MCU Status Register - MCUSR, MCUCSR */
#define    JTD          7
#define    JTRF         4
#define    WDRF         3
#define    BORF         2
#define    EXTRF        1
#define    PORF         0

/* Timer/Counter Control Register (generic) */
#define    FOC          7
#define    WGM0         6
#define    COM1         5
#define    COM0         4
#define    WGM1         3
#define    CS2          2
#define    CS1          1
#define    CS0          0

/* Timer/Counter 0 Control Register - TCCR0 */
#define    TCCR0_FOC0	__port(TCCR0).bit7
#define    TCCR0_WGM00	__port(TCCR0).bit6
#define    TCCR0_COM01	__port(TCCR0).bit5
#define    TCCR0_COM00	__port(TCCR0).bit4
#define    TCCR0_WGM01	__port(TCCR0).bit3
#define    TCCR0_CS02	__port(TCCR0).bit2
#define    TCCR0_CS01	__port(TCCR0).bit1
#define    TCCR0_CS00	__port(TCCR0).bit0

/* Timer/Counter 2 Control Register - TCCR2 */
#define    TCCR2_FOC2	__port(TCCR2).bit7
#define    TCCR2_WGM20  __port(TCCR2).bit6
#define    TCCR2_COM21  __port(TCCR2).bit5
#define    TCCR2_COM20  __port(TCCR2).bit4
#define    TCCR2_WGM21  __port(TCCR2).bit3
#define    TCCR2_CS22   __port(TCCR2).bit2
#define    TCCR2_CS21   __port(TCCR2).bit1
#define    TCCR2_CS20   __port(TCCR2).bit0

/* Timer/Counter 0 Asynchronous Control & Status Register - ASSR */
#define    AS0          3
#define    TCN0UB       2
#define    OCR0UB       1
#define    TCR0UB       0

/* Timer/Counter Control Register A (generic) */
#define    COMA1        7
#define    COMA0        6
#define    COMB1        5
#define    COMB0        4
#define    COMC1        3
#define    COMC0        2
#define    WGMA1        1
#define    WGMA0        0

/* Timer/Counter 1 Control and Status Register A - TCCR1A */
#define    COM1A1       7
#define    COM1A0       6
#define    COM1B1       5
#define    COM1B0       4
#define    COM1C1       3
#define    COM1C0       2
#define    WGM11        1
#define    WGM10        0

/* Timer/Counter 3 Control and Status Register A - TCCR3A */
#define    COM3A1       7
#define    COM3A0       6
#define    COM3B1       5
#define    COM3B0       4
#define    COM3C1       3
#define    COM3C0       2
#define    WGM31        1
#define    WGM30        0

/* Timer/Counter Control and Status Register B (generic) */
#define    ICNC         7
#define    ICES         6
#define    WGMB3        4
#define    WGMB2        3
#define    CSB2         2
#define    CSB1         1
#define    CSB0         0

/* Timer/Counter 1 Control and Status Register B - TCCR1B */
#define    ICNC1        7
#define    ICES1        6
#define    WGM13        4
#define    WGM12        3
#define    CS12         2
#define    CS11         1
#define    CS10         0

/* Timer/Counter 3 Control and Status Register B - TCCR3B */
#define    ICNC3        7
#define    ICES3        6
#define    WGM33        4
#define    WGM32        3
#define    CS32         2
#define    CS31         1
#define    CS30         0

/* Timer/Counter Control Register C (generic) */
#define    FOCA         7
#define    FOCB         6
#define    FOCC         5

/* Timer/Counter 3 Control Register C - TCCR3C */
#define    FOC3A        7
#define    FOC3B        6
#define    FOC3C        5

/* Timer/Counter 1 Control Register C - TCCR1C */
#define    FOC1A        7
#define    FOC1B        6
#define    FOC1C        5

/* On-chip Debug Register - OCDR */
#define    IDRD         7
#define    OCDR7        7
#define    OCDR6        6
#define    OCDR5        5
#define    OCDR4        4
#define    OCDR3        3
#define    OCDR2        2
#define    OCDR1        1
#define    OCDR0        0

/* Watchdog Timer Control Register - WDTCR */
#define    WDCE         4
#define    WDE          3
#define    WDP2         2
#define    WDP1         1
#define    WDP0         0

/* USART0 Control Register B - UCSR0B */
#define    UCSR0B_RXCIE0       __port(UCSR0B).bit7
#define    UCSR0B_TXCIE0       __port(UCSR0B).bit6
#define    UCSR0B_UDRIE0       __port(UCSR0B).bit5
#define    UCSR0B_RXEN0        __port(UCSR0B).bit4
#define    UCSR0B_TXEN0        __port(UCSR0B).bit3
#define    UCSR0B_UCSZ02       __port(UCSR0B).bit2
#define    UCSR0B_RXB80        __port(UCSR0B).bit1
#define    UCSR0B_TXB80        __port(UCSR0B).bit0

/* USART0 Register C - UCSR0C */
#define    UCSR0C_UMSEL0       __port(UCSR0C).bit6
#define    UCSR0C_UPM01        __port(UCSR0C).bit5
#define    UCSR0C_UPM00        __port(UCSR0C).bit4
#define    UCSR0C_USBS0        __port(UCSR0C).bit3
#define    UCSR0C_UCSZ01       __port(UCSR0C).bit2
#define    UCSR0C_UCSZ00       __port(UCSR0C).bit1
#define    UCSR0C_UCPOL0       __port(UCSR0C).bit0


#endif /* PATH_MEGA128_H_ */