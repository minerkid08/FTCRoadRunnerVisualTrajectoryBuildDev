drive.trajectorySequenceBuilder(new Pose2d(-36, -66, Math.toRadians(90)))
	.splineToConstantHeading(new Vector2d(-36, -37), Math.toRadians(90))
	.waitSeconds(1)
	.splineToConstantHeading(new Vector2d(-18, -66), Math.toRadians(-0))
	.waitSeconds(0)
	.splineToConstantHeading(new Vector2d(30, -60), Math.toRadians(-0))
	.splineTo(new Vector2d(50, -36), Math.toRadians(90))
	.turn(Math.toRadians(-180))
	.build();