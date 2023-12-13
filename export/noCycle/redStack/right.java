drive.trajectorySequenceBuilder(new Pose2d(-36, -66, Math.toRadians(90)))
	.lineToLinearHeading(new Pose2d(-30, -36, Math.toRadians(-0)))
	.waitSeconds(0.5)
	.lineToConstantHeading(new Vector2d(-30, -48))
	.splineToConstantHeading(new Vector2d(-24, -60), Math.toRadians(-0))
	.splineToConstantHeading(new Vector2d(12, -60), Math.toRadians(-0))
	.lineToLinearHeading(new Pose2d(48, -36, Math.toRadians(180)))
	.build();