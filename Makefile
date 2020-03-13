predictors:	predictors.o
		g++ -g -Wall -std=c++17 predictors.o -o predictors

predictors.o:	predictors.cpp misc.hh alwaysPredictor.hh bimodalPredictor.hh	\
		gSharePredictor.hh tournamentPredictor.hh btb.hh
		g++ -g -Wall -std=c++17 -c	\
			predictors.cpp		\
			misc.hh			\
			alwaysPredictor.hh	\
			bimodalPredictor.hh	\
			gSharePredictor.hh	\
			tournamentPredictor.hh	\
			btb.hh

clean:	
		rm -f *~ *.o *.gch predictors
