MTGStatGraph: MTGStatGraph.o
	g++ -o MTGStatGraph MTGStatGraph.o
	./MTGStatGraph dimircontrol.txt
	./MTGStatGraph gruuladventures.txt
	./MTGStatGraph rakdosmidrange.txt
	./MTGStatGraph selesnyablink.txt
	./MTGStatGraph temurramp.txt
	jgraph -P -L temurramp.jgr | ps2pdf - | convert -density 300 - -quality 100 TemurRamp.jpg
	jgraph -P -L gruuladventures.jgr | ps2pdf - | convert -density 300 - -quality 100 GruulAdventures.jpg
	jgraph -P -L rakdosmidrange.jgr | ps2pdf - | convert -density 300 - -quality 100 RakdosMidrange.jpg
	jgraph -P -L selesnyablink.jgr | ps2pdf - | convert -density 300 - -quality 100 SelesnyaBlink.jpg
	jgraph -P -L dimircontrol.jgr | ps2pdf - | convert -density 300 - -quality 100 DimirControl.jpg

MTGStatGraph.o: MTGStatGraph.cpp hypergeometric.hpp
	g++ -std=c++11 -c MTGStatGraph.cpp

clean:
	rm -f MTGStatGraph *.o
