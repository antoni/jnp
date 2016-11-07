CXX?=g++
CXXFLAGS+=--std=c++11 -Wall -pedantic -O2

HEADERS=AccountFactory.h Account.h Amount.h BankBuilder.h Bank.h Citizen.h \
		 Currency.h Exceptions.h ExchangeTable.h Galaxy.h \
		 InterstellarBankCommission.h MonthObserver.h InterstellarClock.h \
		 ObservableInterstellarClock.h Planet.h Transaction.h gsb.h

OBJS=Citizen.o Account.o AccountFactory.o Amount.o Transaction.o BankBuilder.o \
	 InterstellarBankCommission.o InterstellarClock.o \
	 ObservableInterstellarClock.o Planet.o Bank.o Currency.o

all: $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f *.o *.dSYM

.PHONY: all clean

