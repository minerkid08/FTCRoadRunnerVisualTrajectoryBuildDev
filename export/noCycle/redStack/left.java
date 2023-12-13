drive.trajectorySequenceBuilder(new Pose2d(-36, -66, Math.toRadians(90)))
	.lineToLinearHeading(new Pose2d(-42, -36, Math.toRadians(180)))
	.waitSeconds(0.5)
	.lineToLinearHeading(new Pose2d(-24, -60, Math.toRadians(-0)))
	.splineToLinearHeading(new Pose2d(12, -60, Math.toRadians(-0)), Math.toRadians(-0))
	.splineTo(new Vector2d(48, -36), Math.toRadians(-0))
	.build();