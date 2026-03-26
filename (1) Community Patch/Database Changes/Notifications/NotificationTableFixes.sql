UPDATE Notifications SET Urgent = 0 WHERE Urgent = 'false';
UPDATE Notifications SET Urgent = 1 WHERE Urgent = 'true';

INSERT INTO Notifications_new
	(ID, NotificationType, Welcomeness, Urgent)
SELECT
	ID, NotificationType, Welcomeness, Urgent
FROM Notifications;

DROP TABLE Notifications;

ALTER TABLE Notifications_new RENAME TO Notifications;
