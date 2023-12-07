drive.trajectorySequenceBuilder(new Pose2d(36, -12, Math.toRadians(90)))
	.splineToConstantHeading(new Vector2d(-18, 6), Math.toRadians(180))
	.splineToConstantHeading(new Vector2d(-54, 36), Math.toRadians(90))
	.turn(Math.toRadians(-180))
	.build();