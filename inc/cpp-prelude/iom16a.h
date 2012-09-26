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

#undef TWBR
typedef IO8<0x00> TWBR_t;
extern TWBR_t TWBR;

#undef TWSR
typedef IO8<0x01> TWSR_t;
extern TWSR_t TWSR;

#undef TWAR
typedef IO8<0x02> TWAR_t;
extern TWAR_t TWAR;

#undef TWDR
typedef IO8<0x03> TWDR_t;
extern TWDR_t TWDR;

#undef ADC
typedef IO16<0x04> ADC_t;
extern ADC_t ADC;

#undef ADCW
typedef IO16<0x04> ADCW_t;
extern ADCW_t ADCW;

#undef ADCL
typedef IO8<0x04> ADCL_t;
extern ADCL_t ADCL;

#undef ADCH
typedef IO8<0x05> ADCH_t;
extern ADCH_t ADCH;

#undef ADCSRA
typedef IO8<0x06> ADCSRA_t;
extern ADCSRA_t ADCSRA;

#undef ADMUX
typedef IO8<0x07> ADMUX_t;
extern ADMUX_t ADMUX;

#undef ACSR
typedef IO8<0x08> ACSR_t;
extern ACSR_t ACSR;

#undef UBRRL
typedef IO8<0x09> UBRRL_t;
extern UBRRL_t UBRRL;

#undef UCSRB
typedef IO8<0x0A> UCSRB_t;
extern UCSRB_t UCSRB;

#undef UCSRA
typedef IO8<0x0B> UCSRA_t;
extern UCSRA_t UCSRA;

#undef UDR
typedef IO8<0x0C> UDR_t;
extern UDR_t UDR;

#undef SPCR
typedef IO8<0x0D> SPCR_t;
extern SPCR_t SPCR;

#undef SPSR
typedef IO8<0x0E> SPSR_t;
extern SPSR_t SPSR;

#undef SPDR
typedef IO8<0x0F> SPDR_t;
extern SPDR_t SPDR;

#undef PIND
typedef IO8<0x10> PIND_t;
extern PIND_t PIND;

#undef DDRD
typedef IO8<0x11> DDRD_t;
extern DDRD_t DDRD;

#undef PORTD
typedef IO8<0x12> PORTD_t;
extern PORTD_t PORTD;

#undef PINC
typedef IO8<0x13> PINC_t;
extern PINC_t PINC;

#undef DDRC
typedef IO8<0x14> DDRC_t;
extern DDRC_t DDRC;

#undef PORTC
typedef IO8<0x15> PORTC_t;
extern PORTC_t PORTC;

#undef PINB
typedef IO8<0x16> PINB_t;
extern PINB_t PINB;

#undef DDRB
typedef IO8<0x17> DDRB_t;
extern DDRB_t DDRB;

#undef PORTB
typedef IO8<0x18> PORTB_t;
extern PORTB_t PORTB;

#undef PINA
typedef IO8<0x19> PINA_t;
extern PINA_t PINA;

#undef DDRA
typedef IO8<0x1A> DDRA_t;
extern DDRA_t DDRA;

#undef PORTA
typedef IO8<0x1B> PORTA_t;
extern PORTA_t PORTA;

#undef EECR
typedef IO8<0x1C> EECR_t;
extern EECR_t EECR;

#undef EEDR
typedef IO8<0x1D> EEDR_t;
extern EEDR_t EEDR;

#undef EEAR
typedef IO16<0x1E> EEAR_t;
extern EEAR_t EEAR;

#undef EEARL
typedef IO8<0x1E> EEARL_t;
extern EEARL_t EEARL;

#undef EEARH
typedef IO8<0x1F> EEARH_t;
extern EEARH_t EEARH;

#undef UBRRH
typedef IO8<0x20> UBRRH_t;
extern UBRRH_t UBRRH;

#undef UCSRC
typedef IO8<0x20> UCSRC_t;
extern UCSRC_t UCSRC;

#undef WDTCR
typedef IO8<0x21> WDTCR_t;
extern WDTCR_t WDTCR;

#undef ASSR
typedef IO8<0x22> ASSR_t;
extern ASSR_t ASSR;

#undef OCR2
typedef IO8<0x23> OCR2_t;
extern OCR2_t OCR2;

#undef TCNT2
typedef IO8<0x24> TCNT2_t;
extern TCNT2_t TCNT2;

#undef TCCR2
typedef IO8<0x25> TCCR2_t;
extern TCCR2_t TCCR2;

#undef ICR1
typedef IO16<0x26> ICR1_t;
extern ICR1_t ICR1;

#undef ICR1L
typedef IO8<0x26> ICR1L_t;
extern ICR1L_t ICR1L;

#undef ICR1H
typedef IO8<0x27> ICR1H_t;
extern ICR1H_t ICR1H;

#undef OCR1B
typedef IO16<0x28> OCR1B_t;
extern OCR1B_t OCR1B;

#undef OCR1BL
typedef IO8<0x28> OCR1BL_t;
extern OCR1BL_t OCR1BL;

#undef OCR1BH
typedef IO8<0x29> OCR1BH_t;
extern OCR1BH_t OCR1BH;

#undef OCR1A
typedef IO16<0x2A> OCR1A_t;
extern OCR1A_t OCR1A;

#undef OCR1AL
typedef IO8<0x2A> OCR1AL_t;
extern OCR1AL_t OCR1AL;

#undef OCR1AH
typedef IO8<0x2B> OCR1AH_t;
extern OCR1AH_t OCR1AH;

#undef TCNT1
typedef IO16<0x2C> TCNT1_t;
extern TCNT1_t TCNT1;

#undef TCNT1L
typedef IO8<0x2C> TCNT1L_t;
extern TCNT1L_t TCNT1L;

#undef TCNT1H
typedef IO8<0x2D> TCNT1H_t;
extern TCNT1H_t TCNT1H;

#undef TCCR1B
typedef IO8<0x2E> TCCR1B_t;
extern TCCR1B_t TCCR1B;

#undef TCCR1A
typedef IO8<0x2F> TCCR1A_t;
extern TCCR1A_t TCCR1A;

#undef SFIOR
typedef IO8<0x30> SFIOR_t;
extern SFIOR_t SFIOR;

#undef OSCCAL
typedef IO8<0x31> OSCCAL_t;
extern OSCCAL_t OSCCAL;

#undef OCDR
typedef IO8<0x31> OCDR_t;
extern OCDR_t OCDR;

#undef TCNT0
typedef IO8<0x32> TCNT0_t;
extern TCNT0_t TCNT0;

#undef TCCR0
typedef IO8<0x33> TCCR0_t;
extern TCCR0_t TCCR0;

#undef MCUCSR
typedef IO8<0x34> MCUCSR_t;
extern MCUCSR_t MCUCSR;

#undef MCUCR
typedef IO8<0x35> MCUCR_t;
extern MCUCR_t MCUCR;

#undef TWCR
typedef IO8<0x36> TWCR_t;
extern TWCR_t TWCR;

#undef SPMCSR
typedef IO8<0x37> SPMCSR_t;
extern SPMCSR_t SPMCSR;

#undef TIFR
typedef IO8<0x38> TIFR_t;
extern TIFR_t TIFR;

#undef TIMSK
typedef IO8<0x39> TIMSK_t;
extern TIMSK_t TIMSK;

#undef GIFR
typedef IO8<0x3A> GIFR_t;
extern GIFR_t GIFR;

#undef GICR
typedef IO8<0x3B> GICR_t;
extern GICR_t GICR;

#undef OCR0
typedef IO8<0x3C> OCR0_t;
extern OCR0_t OCR0;

