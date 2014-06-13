
#include "ActarSimParisGeometry.hh"


G4ThreeVector ActarSimParisGeometry::GetClusterPos(int nr)
{
// All values in mm
G4double detector_side = 56.; 
G4double cluster_side = 3.*detector_side; 
G4double detector_length = 226.; 
G4double space = 5.; 		// thickness of metal support of a cluster 
G4double depth = cluster_side + space + detector_length/2.;	// defines the distance to the face of the cube... 
G4double side = ( cluster_side + space ) / 2.; 


    const G4double cluster_pos[24][3] = 
    {
//					x		z		y   

//  FACE_1 ( towards +X ) 
        { /* cluster_01 */ 		depth, 		 3*side, 	   side }, 
	{ /* cluster_02 */ 		depth, 		side, 		   side }, 
	{ /* cluster_03 */ 		depth, 		side, 		  -side }, 
	{ /* cluster_04 */ 		depth, 		 3*side, 	  -side }, 
//  FACE_2 ( towards +Y ) 
        { /* cluster_05 */		side,  	   	 3*side,  	  depth }, 
	{ /* cluster_06 */		side,  	   	side,  	          depth }, 
	{ /* cluster_07 */		-side,  	side,  	          depth }, 
	{ /* cluster_08 */		-side,  	 3*side,  	  depth }, 
//  FACE_3 ( towards +Z ) 
        { /* cluster_09 */		side,  	   	2*depth,		   side }, 
	{ /* cluster_10 */		side,  	   	2*depth,		  -side }, 
	{ /* cluster_11 */		-side,  	2*depth,		   side }, 
	{ /* cluster_12 */		-side,  	2*depth,		  -side }, 
//  FACE_4 ( towards -X ) 
        { /* cluster_13 */		-depth, 	 3*side,		   side }, 
	{ /* cluster_14 */		-depth, 	 3*side,		  -side }, 
	{ /* cluster_15 */		-depth, 	side,		   side }, 
	{ /* cluster_16 */		-depth, 	side,		  -side }, 
//  FACE_5 ( towards -Y ) 
        { /* cluster_17 */		side,  	   	 3*side,  	 -depth }, 
	{ /* cluster_18 */		side,  	   	side,  	 -depth }, 
	{ /* cluster_19 */		-side,  	 3*side,  	 -depth }, 
	{ /* cluster_20 */		-side,  	side,  	 -depth }, 
//  FACE_6 ( towards -Z ) 
        { /* cluster_21 */		side,  	  	-depth,		   side }, 
	{ /* cluster_22 */		side,  	   	-depth,		  -side }, 
	{ /* cluster_23 */		-side,  	-depth,		   side }, 
	{ /* cluster_24 */		-side,  	-depth,		  -side }, 

    };
//         const G4double cluster_pos[24][3] = 
//     {
// //					x		z		y   

// //  FACE_1 ( towards +X ) 
//         { /* cluster_01 */ 		depth, 		 side, 		   side }, 
// 	{ /* cluster_02 */ 		depth, 		-side, 		   side }, 
// 	{ /* cluster_03 */ 		depth, 		-side, 		  -side }, 
// 	{ /* cluster_04 */ 		depth, 		 side, 		  -side }, 
// //  FACE_2 ( towards +Y ) 
//         { /* cluster_05 */		side,  	   	 side,  	  depth }, 
// 	{ /* cluster_06 */		side,  	   	-side,  	  depth }, 
// 	{ /* cluster_07 */		-side,  	-side,  	  depth }, 
// 	{ /* cluster_08 */		-side,  	 side,  	  depth }, 
// //  FACE_3 ( towards +Z ) 
//         { /* cluster_09 */		side,  	   	depth,		   side }, 
// 	{ /* cluster_10 */		side,  	   	depth,		  -side }, 
// 	{ /* cluster_11 */		-side,  	depth,		   side }, 
// 	{ /* cluster_12 */		-side,  	depth,		  -side }, 
// //  FACE_4 ( towards -X ) 
//         { /* cluster_13 */		-depth, 	 side,		   side }, 
// 	{ /* cluster_14 */		-depth, 	 side,		  -side }, 
// 	{ /* cluster_15 */		-depth, 	-side,		   side }, 
// 	{ /* cluster_16 */		-depth, 	-side,		  -side }, 
// //  FACE_5 ( towards -Y ) 
//         { /* cluster_17 */		side,  	   	 side,  	 -depth }, 
// 	{ /* cluster_18 */		side,  	   	-side,  	 -depth }, 
// 	{ /* cluster_19 */		-side,  	 side,  	 -depth }, 
// 	{ /* cluster_20 */		-side,  	-side,  	 -depth }, 
// //  FACE_6 ( towards -Z ) 
//         { /* cluster_21 */		side,  	  	-depth,		   side }, 
// 	{ /* cluster_22 */		side,  	   	-depth,		  -side }, 
// 	{ /* cluster_23 */		-side,  	-depth,		   side }, 
// 	{ /* cluster_24 */		-side,  	-depth,		  -side }, 

//     };

if( nr<0 || nr>=24 ) 
  //G4Exception("!!! cluster number out of range (must have nr in [0,24])" ); 
  G4Exception("ActarSimParisGeometry::GetClusterPos()","GEOMETRY",FatalException,"!!! cluster number out of range (must have nr in [0,24])" ); 
    
    const G4double* xyz=cluster_pos[nr];
    return G4ThreeVector( xyz[0]*mm, xyz[2]*mm, xyz[1]*mm );
}


G4double ActarSimParisGeometry::GetClusterAngle(int nr)
{
    return GetClusterPos(nr).angle(G4ThreeVector(0,0,1));
}


G4ThreeVector ActarSimParisGeometry::GetDetPos(int nb)
{

    const G4double detector_pos[9][3] = 
    {
//		  x		  z		  y   

        { 	  0.,		  0.,		  0. }, 	// #1	center-center 
        { 	  0., 		 56., 		  0. }, 	// #2	up-center 
        { 	 56., 		 56., 		  0. }, 	// #3	up-right 
        {	 56., 		  0., 		  0. }, 	// #4	center-right 
        {	 56., 		-56., 		  0. }, 	// #5	down-right 
        {	  0., 		-56., 		  0. },		// #6	down-center 
        {	-56., 		-56., 		  0. },		// #7	down-left 
        {	-56., 		  0., 		  0. }, 	// #8	center-left 
        {	-56., 		 56., 		  0. },		// #9	up-left 

// it should look like this (inside a cluster): 
//
//	9 2 3 
//	8 1 4 
//	7 6 5 
    }; 

if( nb<0 || nb>=9 ) 
  //G4Exception("!!! detector number (inside the cluster) out of range (should be in [0,9])" ); 
    G4Exception("ActarSimParisGeometry::GetDetPos()","GEOMETRY",FatalException,"!!! detector number (inside the cluster) out of range (should be in [0,9])" ); 
    
    const G4double* det = detector_pos[nb]; 
    return G4ThreeVector( det[0]*mm, det[1]*mm, det[2]*mm ); 
}
