Compile:
		g++ -pthread tas.cpp -o tas.out;
		g++ -pthread cas.cpp -o cas.out;
		g++ -pthread cas_bound.cpp -o cas_b.out;

		./tas.out;
		./cas.out;
		./cas_b.out;

clean:
		rm CAS-log.txt TAS-log.txt CAS_B-log.txt *.out;
