### FAnToM Session
### API Version: 20140515
### Used core version:    GITDIR-NOTFOUND (GITDIR-NOTFOUND)
### Used toolbox version: GITDIR-NOTFOUND (GITDIR-NOTFOUND)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(0, 0, 3.8637), fantom.math.Vector3(0, 0, 2.8637), fantom.math.Vector3(0, 1, 0), 1, 3.16202e-322 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(0, 3.8637, 0), fantom.math.Vector3(0, 2.8637, 0), fantom.math.Vector3(0, 0, 1), 0, 1 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, 0, 3.8637), fantom.math.Vector3(0, 0, 2.8637), fantom.math.Vector3(0, 1, 0), 0, 1 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(3.8637, 0, 0), fantom.math.Vector3(2.8637, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1 ) )

fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 0, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 1, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 2, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 3, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 4, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 5, fantom.math.Matrix4( (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) ), False ) )

fantom.ui.setBackgroundColor( fantom.math.Color(0, 0, 0, 1) )

fantom.ui.setRotationCenter( fantom.ui.RotationCenter( fantom.math.Vector3(0, 0, 0), True, True, True ) )


################################################################
###                  Create algorithms                       ###
################################################################
Wall = fantom.makeAlgorithm("Wall")
Wall.setName( "Wall" )
Wall.setOption("Input depth file", "/home/ava/Desktop/depth_4335288162626.png")
Wall.setOption("Input color file", "")
fantom.ui.setAlgorithmPosition(Wall, fantom.math.Vector2(0, 35))

Camera = fantom.makeAlgorithm("Camera")
Camera.setName( "Camera" )
fantom.ui.setAlgorithmPosition(Camera, fantom.math.Vector2(0, 163))
Camera.setVisualOutputVisible('triangle', True)



################################################################
###                     Make Connections                     ###
################################################################
Wall.connect("depth", Camera, "field")


################################################################
###                      Run algorithms                      ###
################################################################
fantom.scheduleAllNecessary()