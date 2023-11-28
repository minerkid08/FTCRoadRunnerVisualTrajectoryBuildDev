drive.trajectorySequenceBuilder(new Pose2d(-36, 66, Math.toRadians(270)))
	.splineTo(new Vector2d(-56, 32), Math.toRadians(180))
	.lineToLinearHeading(new Pose2d(-60, 36, 360))
	.splineTo(new Vector2d(-24, 60), Math.toRadians(360))
	.splineTo(new Vector2d(12, 60), Math.toRadians(360))
	.splineTo(new Vector2d(48, 36), Math.toRadians(360))
	.turn(Math.toRadians(90))
	.splineTo(new Vector2d(12, 60), Math.toRadians(180))
	.splineTo(new Vector2d(-24, 60), Math.toRadians(180))
	.lineToLinearHeading(new Pose2d(-60, 36, 360))
	.splineTo(new Vector2d(-24, 60), Math.toRadians(360))
	.splineTo(new Vector2d(12, 60), Math.toRadians(360))
	.splineTo(new Vector2d(48, 36), Math.toRadians(360))
	.turn(Math.toRadians(90))
	.splineTo(new Vector2d(12, 60), Math.toRadians(180))
	.splineTo(new Vector2d(-24, 60), Math.toRadians(180))
	.lineToLinearHeading(new Pose2d(-60, 36, 360))
	.splineTo(new Vector2d(-24, 60), Math.toRadians(360))
	.splineTo(new Vector2d(12, 60), Math.toRadians(360))
	.splineTo(new Vector2d(48, 36), Math.toRadians(360))
	.build();