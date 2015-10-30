Library Dependencies:
	boost-chrono
	boost-asio
	boost-regex
	boost-thread
	boost-system
	boost-date-time
	boost-property-tree
	Eigen
	qmake

Install deps for linux ubuntu:
	sudo apr-get install libeigen3-dev libboost-all-dev
	
Install deps for MS Visual Studio 2015 nuget (in pakages.config file):
    Update-Package -reinstall