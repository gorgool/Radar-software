TEMPLATE = subdirs

SUBDIRS = Utils CoordinateSystems Shared SatelliteUtils Client Server ConfigManager Logger

Utils.file = ../Utils/Utils.pro

CoordinateSystems.file = ../CoordinateSystems/CoordinateSystems.pro

Shared.file = Shared/Shared.pro

SatelliteUtils.file = SatelliteUtils/SatelliteUtils.pro

ConfigManager.file = ../ConfigManager/ConfigManager.pro

Logger.file = ../Logger/Logger.pro

Client.file = Client/Client.pro
Client.depends = Shared Utils ConfigManager Logger

Server.file = Server/Server.pro
Server.depends = Shared Utils SatelliteUtils CoordinateSystems ConfigManager Logger

