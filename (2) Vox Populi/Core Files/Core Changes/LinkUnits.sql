INSERT INTO Language_en_US (Tag, Text) VALUES 
('TXT_KEY_ACTION_LINK_UNITS', "Link units on this plot."),
('TXT_KEY_ACTION_UNLINK_UNITS', "Unlink units.");


UPDATE InterfaceModes SET Visible = 1, HotKey = "KB_X", AltDown = 0, SelectAll = 0, IconIndex = 0, IconAtlas = "GROUP_MOVE_ATLAS" WHERE Type = "INTERFACEMODE_MOVE_TO_ALL";
UPDATE Language_en_US SET Text = "Group Move" WHERE Tag = "TXT_KEY_INTERFACEMODE_MOVE_TO_ALL";
UPDATE Language_en_US SET Text = "Order the selected unit and adjacent combat units of the same domain to move." WHERE Tag = "TXT_KEY_INTERFACEMODE_MOVE_TO_ALL_HELP";
