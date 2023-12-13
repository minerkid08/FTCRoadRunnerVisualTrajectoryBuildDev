drive.trajectorySequenceBuilder(new Pose2d(12, -66, Math.toRadians(90)))
	.lineToLinearHeading(new Pose2d(30, -36, Math.toRadians(180)))
	.waitSeconds(0.5)
	.lineToLinearHeading(new Pose2d(48, -36, Math.toRadians(-0)))
	.build();