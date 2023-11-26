drive.trajectorySequenceBuilder(new Pose2d(12, 66, Math.toRadians(-90)))
	.splineTo(new Vector2d(12, 36), Math.toRadians(-90))
	.turn(Math.toRadians(90))
	.splineTo(new Vector2d(48, 36), Math.toRadians(-0))
	.turn(Math.toRadians(90))
	.splineTo(new Vector2d(48, 60), Math.toRadians(90))
	.turn(Math.toRadians(-90))
	.splineTo(new Vector2d(66, 60), Math.toRadians(-0))
	.build();