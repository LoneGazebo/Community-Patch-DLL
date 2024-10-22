-- Build times (to compensate for build turn fix)
UPDATE Builds
SET Time = Time - 100
WHERE Time > 0;

UPDATE BuildFeatures
SET Time = Time - 100
WHERE Time > 0;
