predictors:	predictors.o
		g++ -g -Wall -std=c++17 predictors.o -o predictors

predictors.o:	predictors.cpp misc.h alwaysPredictor.h bimodalPredictor.h	\
		gSharePredictor.h tournamentPredictor.h btb.h
		g++ -g -Wall -std=c++17 -c	\
			predictors.cpp		\
			misc.h			\
			alwaysPredictor.h	\
			bimodalPredictor.h	\
			gSharePredictor.h	\
			tournamentPredictor.h	\
			btb.h

clean:	
		rm -f *~ *.o *.gch predictors
