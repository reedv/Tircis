VERSION=v0

default:
	cd ./tircis_process_cmd_$(VERSION); make
	cd ./testdata; make

clean:
	cd ./tircis_process_cmd_$(VERSION); make clean
	cd ./testdata; make clean
