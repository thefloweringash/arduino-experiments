// AUTO GENERATED HEADER FILE

// Prevent header being included twice
#if defined(CPP_PRELUDE_INCLUDED_CPU_HEADER)
#  error Already included header. Please include only cppprelude.h
#endif

#define CPP_PRELUDE_INCLUDED_CPU_HEADER 1

// Prevent header being used without cppprelude.h
#if !defined(CPP_PRELUDE_H)
#  error Missing cppprelude.h. Please include only cppprelude.h
#endif

#undef PINB
typedef IO8<0x03> PINB_t;
extern PINB_t PINB;

#undef DDRB
typedef IO8<0x04> DDRB_t;
extern DDRB_t DDRB;

#undef PORTB
typedef IO8<0x05> PORTB_t;
extern PORTB_t PORTB;

#undef PINC
typedef IO8<0x06> PINC_t;
extern PINC_t PINC;

#undef DDRC
typedef IO8<0x07> DDRC_t;
extern DDRC_t DDRC;

#undef PORTC
typedef IO8<0x08> PORTC_t;
extern PORTC_t PORTC;

#undef PIND
typedef IO8<0x09> PIND_t;
extern PIND_t PIND;

#undef DDRD
typedef IO8<0x0A> DDRD_t;
extern DDRD_t DDRD;

#undef PORTD
typedef IO8<0x0B> PORTD_t;
extern PORTD_t PORTD;

#undef TIFR0
typedef IO8<0x15> TIFR0_t;
extern TIFR0_t TIFR0;

#undef TIFR1
typedef IO8<0x16> TIFR1_t;
extern TIFR1_t TIFR1;

#undef TIFR2
typedef IO8<0x17> TIFR2_t;
extern TIFR2_t TIFR2;

#undef PCIFR
typedef IO8<0x1B> PCIFR_t;
extern PCIFR_t PCIFR;

#undef EIFR
typedef IO8<0x1C> EIFR_t;
extern EIFR_t EIFR;

#undef EIMSK
typedef IO8<0x1D> EIMSK_t;
extern EIMSK_t EIMSK;

#undef GPIOR0
typedef IO8<0x1E> GPIOR0_t;
extern GPIOR0_t GPIOR0;

#undef EECR
typedef IO8<0x1F> EECR_t;
extern EECR_t EECR;

#undef EEDR
typedef IO8<0x20> EEDR_t;
extern EEDR_t EEDR;

#undef EEAR
typedef IO16<0x21> EEAR_t;
extern EEAR_t EEAR;

#undef EEARL
typedef IO8<0x21> EEARL_t;
extern EEARL_t EEARL;

#undef EEARH
typedef IO8<0x22> EEARH_t;
extern EEARH_t EEARH;

#undef GTCCR
typedef IO8<0x23> GTCCR_t;
extern GTCCR_t GTCCR;

#undef TCCR0A
typedef IO8<0x24> TCCR0A_t;
extern TCCR0A_t TCCR0A;

#undef TCCR0B
typedef IO8<0x25> TCCR0B_t;
extern TCCR0B_t TCCR0B;

#undef TCNT0
typedef IO8<0x26> TCNT0_t;
extern TCNT0_t TCNT0;

#undef OCR0A
typedef IO8<0x27> OCR0A_t;
extern OCR0A_t OCR0A;

#undef OCR0B
typedef IO8<0x28> OCR0B_t;
extern OCR0B_t OCR0B;

#undef GPIOR1
typedef IO8<0x2A> GPIOR1_t;
extern GPIOR1_t GPIOR1;

#undef GPIOR2
typedef IO8<0x2B> GPIOR2_t;
extern GPIOR2_t GPIOR2;

#undef SPCR
typedef IO8<0x2C> SPCR_t;
extern SPCR_t SPCR;

#undef SPSR
typedef IO8<0x2D> SPSR_t;
extern SPSR_t SPSR;

#undef SPDR
typedef IO8<0x2E> SPDR_t;
extern SPDR_t SPDR;

#undef ACSR
typedef IO8<0x30> ACSR_t;
extern ACSR_t ACSR;

#undef SMCR
typedef IO8<0x33> SMCR_t;
extern SMCR_t SMCR;

#undef MCUSR
typedef IO8<0x34> MCUSR_t;
extern MCUSR_t MCUSR;

#undef MCUCR
typedef IO8<0x35> MCUCR_t;
extern MCUCR_t MCUCR;

#undef SPMCSR
typedef IO8<0x37> SPMCSR_t;
extern SPMCSR_t SPMCSR;

#undef WDTCSR
typedef Mem8<0x60> WDTCSR_t;
extern WDTCSR_t WDTCSR;

#undef CLKPR
typedef Mem8<0x61> CLKPR_t;
extern CLKPR_t CLKPR;

#undef PRR
typedef Mem8<0x64> PRR_t;
extern PRR_t PRR;

#undef OSCCAL
typedef Mem8<0x66> OSCCAL_t;
extern OSCCAL_t OSCCAL;

#undef PCICR
typedef Mem8<0x68> PCICR_t;
extern PCICR_t PCICR;

#undef EICRA
typedef Mem8<0x69> EICRA_t;
extern EICRA_t EICRA;

#undef PCMSK0
typedef Mem8<0x6B> PCMSK0_t;
extern PCMSK0_t PCMSK0;

#undef PCMSK1
typedef Mem8<0x6C> PCMSK1_t;
extern PCMSK1_t PCMSK1;

#undef PCMSK2
typedef Mem8<0x6D> PCMSK2_t;
extern PCMSK2_t PCMSK2;

#undef TIMSK0
typedef Mem8<0x6E> TIMSK0_t;
extern TIMSK0_t TIMSK0;

#undef TIMSK1
typedef Mem8<0x6F> TIMSK1_t;
extern TIMSK1_t TIMSK1;

#undef TIMSK2
typedef Mem8<0x70> TIMSK2_t;
extern TIMSK2_t TIMSK2;

#undef ADC
typedef Mem16<0x78> ADC_t;
extern ADC_t ADC;

#undef ADCW
typedef Mem16<0x78> ADCW_t;
extern ADCW_t ADCW;

#undef ADCL
typedef Mem8<0x78> ADCL_t;
extern ADCL_t ADCL;

#undef ADCH
typedef Mem8<0x79> ADCH_t;
extern ADCH_t ADCH;

#undef ADCSRA
typedef Mem8<0x7A> ADCSRA_t;
extern ADCSRA_t ADCSRA;

#undef ADCSRB
typedef Mem8<0x7B> ADCSRB_t;
extern ADCSRB_t ADCSRB;

#undef ADMUX
typedef Mem8<0x7C> ADMUX_t;
extern ADMUX_t ADMUX;

#undef DIDR0
typedef Mem8<0x7E> DIDR0_t;
extern DIDR0_t DIDR0;

#undef DIDR1
typedef Mem8<0x7F> DIDR1_t;
extern DIDR1_t DIDR1;

#undef TCCR1A
typedef Mem8<0x80> TCCR1A_t;
extern TCCR1A_t TCCR1A;

#undef TCCR1B
typedef Mem8<0x81> TCCR1B_t;
extern TCCR1B_t TCCR1B;

#undef TCCR1C
typedef Mem8<0x82> TCCR1C_t;
extern TCCR1C_t TCCR1C;

#undef TCNT1
typedef Mem16<0x84> TCNT1_t;
extern TCNT1_t TCNT1;

#undef TCNT1L
typedef Mem8<0x84> TCNT1L_t;
extern TCNT1L_t TCNT1L;

#undef TCNT1H
typedef Mem8<0x85> TCNT1H_t;
extern TCNT1H_t TCNT1H;

#undef ICR1
typedef Mem16<0x86> ICR1_t;
extern ICR1_t ICR1;

#undef ICR1L
typedef Mem8<0x86> ICR1L_t;
extern ICR1L_t ICR1L;

#undef ICR1H
typedef Mem8<0x87> ICR1H_t;
extern ICR1H_t ICR1H;

#undef OCR1A
typedef Mem16<0x88> OCR1A_t;
extern OCR1A_t OCR1A;

#undef OCR1AL
typedef Mem8<0x88> OCR1AL_t;
extern OCR1AL_t OCR1AL;

#undef OCR1AH
typedef Mem8<0x89> OCR1AH_t;
extern OCR1AH_t OCR1AH;

#undef OCR1B
typedef Mem16<0x8A> OCR1B_t;
extern OCR1B_t OCR1B;

#undef OCR1BL
typedef Mem8<0x8A> OCR1BL_t;
extern OCR1BL_t OCR1BL;

#undef OCR1BH
typedef Mem8<0x8B> OCR1BH_t;
extern OCR1BH_t OCR1BH;

#undef TCCR2A
typedef Mem8<0xB0> TCCR2A_t;
extern TCCR2A_t TCCR2A;

#undef TCCR2B
typedef Mem8<0xB1> TCCR2B_t;
extern TCCR2B_t TCCR2B;

#undef TCNT2
typedef Mem8<0xB2> TCNT2_t;
extern TCNT2_t TCNT2;

#undef OCR2A
typedef Mem8<0xB3> OCR2A_t;
extern OCR2A_t OCR2A;

#undef OCR2B
typedef Mem8<0xB4> OCR2B_t;
extern OCR2B_t OCR2B;

#undef ASSR
typedef Mem8<0xB6> ASSR_t;
extern ASSR_t ASSR;

#undef TWBR
typedef Mem8<0xB8> TWBR_t;
extern TWBR_t TWBR;

#undef TWSR
typedef Mem8<0xB9> TWSR_t;
extern TWSR_t TWSR;

#undef TWAR
typedef Mem8<0xBA> TWAR_t;
extern TWAR_t TWAR;

#undef TWDR
typedef Mem8<0xBB> TWDR_t;
extern TWDR_t TWDR;

#undef TWCR
typedef Mem8<0xBC> TWCR_t;
extern TWCR_t TWCR;

#undef TWAMR
typedef Mem8<0xBD> TWAMR_t;
extern TWAMR_t TWAMR;

#undef UCSR0A
typedef Mem8<0xC0> UCSR0A_t;
extern UCSR0A_t UCSR0A;

#undef UCSR0B
typedef Mem8<0xC1> UCSR0B_t;
extern UCSR0B_t UCSR0B;

#undef UCSR0C
typedef Mem8<0xC2> UCSR0C_t;
extern UCSR0C_t UCSR0C;

#undef UBRR0
typedef Mem16<0xC4> UBRR0_t;
extern UBRR0_t UBRR0;

#undef UBRR0L
typedef Mem8<0xC4> UBRR0L_t;
extern UBRR0L_t UBRR0L;

#undef UBRR0H
typedef Mem8<0xC5> UBRR0H_t;
extern UBRR0H_t UBRR0H;

#undef UDR0
typedef Mem8<0xC6> UDR0_t;
extern UDR0_t UDR0;

