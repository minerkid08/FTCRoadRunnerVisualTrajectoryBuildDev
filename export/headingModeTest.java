drive.trajectorySequenceBuilder(new Pose2d(-36, -66, Math.toRadians(90)))
	.splineToLinearHeading(new Vector2d(-36, -36), Math.toRadians(-0))
	.splineToConstantHeading(new Vector2d(-36, 12), Math.toRadians(-90))
	.splineToSplineHeading(new Vector2d(-36, 60), Math.toRadians(-180))
	.build();