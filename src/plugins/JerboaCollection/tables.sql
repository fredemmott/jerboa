/* IMPORTANT - \n\n *MUST* separate commands */ 
CREATE TABLE `Albums` (
	`ID` INTEGER PRIMARY KEY,
	`Artist` INTEGER NOT NULL,
	`Name`	TEXT NOT NULL,
	`SortKey` TEXT NOT NULL
);

CREATE TABLE `Artists` (
	`ID` INTEGER PRIMARY KEY,
	`Name` TEXT NOT NULL,
	`RomanisedName` TEXT NOT NULL,
	`SortKey` TEXT NOT NULL
);

CREATE TABLE `Tracks` (
	`ID` INTEGER PRIMARY KEY,
	`FileName` TEXT NOT NULL,
	`Album` INTEGER NOT NULL,
	`Artist` INTEGER NOT NULL,
	`Name` TEXT NOT NULL,
	`TrackNumber` INT NOT NULL,
	`AlbumRG` FLOAT NOT NULL,
	`SearchKey` TEXT NOT NULL,
	`TrackRG` FLOAT NOT NULL,
	`MusicBrainzTrackID` CHAR(36) NOT NULL
);

CREATE VIEW TrackData AS
	SELECT
		Tracks.ID AS ID,
		Tracks.FileName AS FileName,
		Albums.Name AS Album,
		AlbumArtist.Name AS AlbumArtist,
		AlbumArtist.RomanisedName AS AlbumArtistRomanised,
		TrackArtist.Name AS Artist,
		TrackArtist.RomanisedName AS ArtistRomanised,
		Tracks.Name AS Title,
		Tracks.TrackNumber AS TrackNumber,
		Tracks.AlbumRG AS AlbumRG,
		Tracks.TrackRG AS TrackRG,
		Tracks.MusicBrainzTrackID AS MBID,
		Tracks.SearchKey AS SearchKey
	FROM
		Tracks
		JOIN Albums ON Albums.ID = Tracks.Album
		JOIN Artists AS TrackArtist ON Tracks.Artist = TrackArtist.ID
		JOIN Artists AS AlbumArtist ON Albums.Artist = AlbumArtist.ID;

/* For MySQL - SQLite Will Barf */
ALTER TABLE `Albums` ADD KEY(`Artist`);

ALTER TABLE `Albums` CHANGE COLUMN `ID` `ID` INTEGER AUTO_INCREMENT;

ALTER TABLE `Albums` CONVERT TO CHARACTER SET UTF8;

ALTER TABLE `Artists` CHANGE COLUMN `ID` `ID` INTEGER AUTO_INCREMENT;

ALTER TABLE `Artists` CONVERT TO CHARACTER SET UTF8;

ALTER TABLE `Tracks` CHANGE COLUMN `ID` `ID` INTEGER AUTO_INCREMENT;

ALTER TABLE `Tracks` CONVERT TO CHARACTER SET UTF8;

ALTER TABLE `Tracks` ADD KEY(`Artist`);

ALTER TABLE `Tracks` ADD KEY(`Album`);
