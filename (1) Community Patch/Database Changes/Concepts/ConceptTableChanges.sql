-- The text keys of contents in the "Extended Information" and "Designer's Notes" boxes in concept articles in the Civilopedia
ALTER TABLE Concepts ADD Extended text REFERENCES Language_en_US (Tag);
ALTER TABLE Concepts ADD DesignNotes text REFERENCES Language_en_US (Tag);
