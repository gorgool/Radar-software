TEMPLATE = subdirs

SUBDIRS = Utils CoordinateSystems Shared SatelliteUtils Client Server

Utils.file = ../Utils/Utils.pro

CoordinateSystems.file = ../CoordinateSystems/CoordinateSystems.pro

Shared.file = Shared/Shared.pro

SatelliteUtils.file = Satellite_Utils/SatelliteUtils.pro

Client.file = Client/Client.pro
Client.depends = Shared Utils

Server.file = Server/Server.pro
Server.depends = Shared Utils SatelliteUtils CoordinateSystems
