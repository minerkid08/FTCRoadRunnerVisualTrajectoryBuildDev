drive.trajectorySequenceBuilder(new Pose2d(48, -36, Math.toRadians(180)))
	.splineTo(new Vector2d(12, -60), Math.toRadians(180))
	.splineTo(new Vector2d(-24, -60), Math.toRadians(180))
	.lineToLinearHeading(new Pose2d(-60, -36, -0))
	.splineTo(new Vector2d(36, 12), Math.toRadians(90))
	.build();