\ Model Sheep
\ LP format - for model browsing. Use MPS format to capture full model detail.
Minimize
 [ 2 xa,0 * ca,0 + 2 xa,0 * ca,1 + 2 xa,0 * ca,2 + 2 xa,1 * ca,1
   + 2 xa,1 * ca,2 + 2 xa,1 * ca,3 + 2 xa,2 * ca,2 + 2 xa,2 * ca,3
   + 2 xa,2 * ca,4 + 2 xa,3 * ca,3 + 2 xa,3 * ca,4 + 2 xa,3 * ca,5
   + 2 xa,4 * ca,4 + 2 xa,4 * ca,5 + 2 xa,4 * ca,6 + 2 xa,5 * ca,5
   + 2 xa,5 * ca,6 + 2 xa,5 * ca,7 + 2 xa,6 * ca,6 + 2 xa,6 * ca,7
   + 2 xa,6 * ca,8 + 2 xa,7 * ca,7 + 2 xa,7 * ca,8 + 2 xa,7 * ca,9
   + 2 xa,8 * ca,8 + 2 xa,8 * ca,9 + 2 xa,9 * ca,9 + 2 xb,0 * cb,0
   + 2 xb,0 * cb,1 + 2 xb,0 * cb,2 + 2 xb,1 * cb,1 + 2 xb,1 * cb,2
   + 2 xb,1 * cb,3 + 2 xb,2 * cb,2 + 2 xb,2 * cb,3 + 2 xb,2 * cb,4
   + 2 xb,3 * cb,3 + 2 xb,3 * cb,4 + 2 xb,3 * cb,5 + 2 xb,4 * cb,4
   + 2 xb,4 * cb,5 + 2 xb,4 * cb,6 + 2 xb,5 * cb,5 + 2 xb,5 * cb,6
   + 2 xb,5 * cb,7 + 2 xb,6 * cb,6 + 2 xb,6 * cb,7 + 2 xb,6 * cb,8
   + 2 xb,7 * cb,7 + 2 xb,7 * cb,8 + 2 xb,7 * cb,9 + 2 xb,8 * cb,8
   + 2 xb,8 * cb,9 + 2 xb,9 * cb,9 + 2 xc,0 * cc,0 + 2 xc,0 * cc,1
   + 2 xc,0 * cc,2 + 2 xc,1 * cc,1 + 2 xc,1 * cc,2 + 2 xc,1 * cc,3
   + 2 xc,2 * cc,2 + 2 xc,2 * cc,3 + 2 xc,2 * cc,4 + 2 xc,3 * cc,3
   + 2 xc,3 * cc,4 + 2 xc,3 * cc,5 + 2 xc,4 * cc,4 + 2 xc,4 * cc,5
   + 2 xc,4 * cc,6 + 2 xc,5 * cc,5 + 2 xc,5 * cc,6 + 2 xc,5 * cc,7
   + 2 xc,6 * cc,6 + 2 xc,6 * cc,7 + 2 xc,6 * cc,8 + 2 xc,7 * cc,7
   + 2 xc,7 * cc,8 + 2 xc,7 * cc,9 + 2 xc,8 * cc,8 + 2 xc,8 * cc,9
   + 2 xc,9 * cc,9 ] / 2 
Subject To
 (1): 3 xa,0 - ca,0 <= 0
 (4): ca,0 >= 0
 (5): 3 xa,0 - ca,0 >= 0
 (1): 3 xa,1 - ca,1 <= 0
 (2): 2 xa,0 - ca,1 <= 0
 (4): ca,1 >= 0
 (5): 2 xa,0 + 3 xa,1 - ca,1 >= 0
 (1): 3 xa,2 - ca,2 <= 0
 (2): 2 xa,1 - ca,2 <= 0
 (3): xa,0 - ca,2 <= 0
 (4): ca,2 >= 0
 (5): xa,0 + 2 xa,1 + 3 xa,2 - ca,2 >= 0
 (1): 3 xa,3 - ca,3 <= 0
 (2): 2 xa,2 - ca,3 <= 0
 (3): xa,1 - ca,3 <= 0
 (4): ca,3 >= 0
 (5): xa,1 + 2 xa,2 + 3 xa,3 - ca,3 >= 0
 (1): 3 xa,4 - ca,4 <= 0
 (2): 2 xa,3 - ca,4 <= 0
 (3): xa,2 - ca,4 <= 0
 (4): ca,4 >= 0
 (5): xa,2 + 2 xa,3 + 3 xa,4 - ca,4 >= 0
 (1): 3 xa,5 - ca,5 <= 0
 (2): 2 xa,4 - ca,5 <= 0
 (3): xa,3 - ca,5 <= 0
 (4): ca,5 >= 0
 (5): xa,3 + 2 xa,4 + 3 xa,5 - ca,5 >= 0
 (1): 3 xa,6 - ca,6 <= 0
 (2): 2 xa,5 - ca,6 <= 0
 (3): xa,4 - ca,6 <= 0
 (4): ca,6 >= 0
 (5): xa,4 + 2 xa,5 + 3 xa,6 - ca,6 >= 0
 (1): 3 xa,7 - ca,7 <= 0
 (2): 2 xa,6 - ca,7 <= 0
 (3): xa,5 - ca,7 <= 0
 (4): ca,7 >= 0
 (5): xa,5 + 2 xa,6 + 3 xa,7 - ca,7 >= 0
 (1): 3 xa,8 - ca,8 <= 0
 (2): 2 xa,7 - ca,8 <= 0
 (3): xa,6 - ca,8 <= 0
 (4): ca,8 >= 0
 (5): xa,6 + 2 xa,7 + 3 xa,8 - ca,8 >= 0
 (1): 3 xa,9 - ca,9 <= 0
 (2): 2 xa,8 - ca,9 <= 0
 (3): xa,7 - ca,9 <= 0
 (4): ca,9 >= 0
 (5): xa,7 + 2 xa,8 + 3 xa,9 - ca,9 >= 0
 (1): 3 xb,0 - cb,0 <= 0
 (4): cb,0 >= 0
 (5): 3 xb,0 - cb,0 >= 0
 (1): 3 xb,1 - cb,1 <= 0
 (2): 2 xb,0 - cb,1 <= 0
 (4): cb,1 >= 0
 (5): 2 xb,0 + 3 xb,1 - cb,1 >= 0
 (1): 3 xb,2 - cb,2 <= 0
 (2): 2 xb,1 - cb,2 <= 0
 (3): xb,0 - cb,2 <= 0
 (4): cb,2 >= 0
 (5): xb,0 + 2 xb,1 + 3 xb,2 - cb,2 >= 0
 (1): 3 xb,3 - cb,3 <= 0
 (2): 2 xb,2 - cb,3 <= 0
 (3): xb,1 - cb,3 <= 0
 (4): cb,3 >= 0
 (5): xb,1 + 2 xb,2 + 3 xb,3 - cb,3 >= 0
 (1): 3 xb,4 - cb,4 <= 0
 (2): 2 xb,3 - cb,4 <= 0
 (3): xb,2 - cb,4 <= 0
 (4): cb,4 >= 0
 (5): xb,2 + 2 xb,3 + 3 xb,4 - cb,4 >= 0
 (1): 3 xb,5 - cb,5 <= 0
 (2): 2 xb,4 - cb,5 <= 0
 (3): xb,3 - cb,5 <= 0
 (4): cb,5 >= 0
 (5): xb,3 + 2 xb,4 + 3 xb,5 - cb,5 >= 0
 (1): 3 xb,6 - cb,6 <= 0
 (2): 2 xb,5 - cb,6 <= 0
 (3): xb,4 - cb,6 <= 0
 (4): cb,6 >= 0
 (5): xb,4 + 2 xb,5 + 3 xb,6 - cb,6 >= 0
 (1): 3 xb,7 - cb,7 <= 0
 (2): 2 xb,6 - cb,7 <= 0
 (3): xb,5 - cb,7 <= 0
 (4): cb,7 >= 0
 (5): xb,5 + 2 xb,6 + 3 xb,7 - cb,7 >= 0
 (1): 3 xb,8 - cb,8 <= 0
 (2): 2 xb,7 - cb,8 <= 0
 (3): xb,6 - cb,8 <= 0
 (4): cb,8 >= 0
 (5): xb,6 + 2 xb,7 + 3 xb,8 - cb,8 >= 0
 (1): 3 xb,9 - cb,9 <= 0
 (2): 2 xb,8 - cb,9 <= 0
 (3): xb,7 - cb,9 <= 0
 (4): cb,9 >= 0
 (5): xb,7 + 2 xb,8 + 3 xb,9 - cb,9 >= 0
 (1): 3 xc,0 - cc,0 <= 0
 (4): cc,0 >= 0
 (5): 3 xc,0 - cc,0 >= 0
 (1): 3 xc,1 - cc,1 <= 0
 (2): 2 xc,0 - cc,1 <= 0
 (4): cc,1 >= 0
 (5): 2 xc,0 + 3 xc,1 - cc,1 >= 0
 (1): 3 xc,2 - cc,2 <= 0
 (2): 2 xc,1 - cc,2 <= 0
 (3): xc,0 - cc,2 <= 0
 (4): cc,2 >= 0
 (5): xc,0 + 2 xc,1 + 3 xc,2 - cc,2 >= 0
 (1): 3 xc,3 - cc,3 <= 0
 (2): 2 xc,2 - cc,3 <= 0
 (3): xc,1 - cc,3 <= 0
 (4): cc,3 >= 0
 (5): xc,1 + 2 xc,2 + 3 xc,3 - cc,3 >= 0
 (1): 3 xc,4 - cc,4 <= 0
 (2): 2 xc,3 - cc,4 <= 0
 (3): xc,2 - cc,4 <= 0
 (4): cc,4 >= 0
 (5): xc,2 + 2 xc,3 + 3 xc,4 - cc,4 >= 0
 (1): 3 xc,5 - cc,5 <= 0
 (2): 2 xc,4 - cc,5 <= 0
 (3): xc,3 - cc,5 <= 0
 (4): cc,5 >= 0
 (5): xc,3 + 2 xc,4 + 3 xc,5 - cc,5 >= 0
 (1): 3 xc,6 - cc,6 <= 0
 (2): 2 xc,5 - cc,6 <= 0
 (3): xc,4 - cc,6 <= 0
 (4): cc,6 >= 0
 (5): xc,4 + 2 xc,5 + 3 xc,6 - cc,6 >= 0
 (1): 3 xc,7 - cc,7 <= 0
 (2): 2 xc,6 - cc,7 <= 0
 (3): xc,5 - cc,7 <= 0
 (4): cc,7 >= 0
 (5): xc,5 + 2 xc,6 + 3 xc,7 - cc,7 >= 0
 (1): 3 xc,8 - cc,8 <= 0
 (2): 2 xc,7 - cc,8 <= 0
 (3): xc,6 - cc,8 <= 0
 (4): cc,8 >= 0
 (5): xc,6 + 2 xc,7 + 3 xc,8 - cc,8 >= 0
 (1): 3 xc,9 - cc,9 <= 0
 (2): 2 xc,8 - cc,9 <= 0
 (3): xc,7 - cc,9 <= 0
 (4): cc,9 >= 0
 (5): xc,7 + 2 xc,8 + 3 xc,9 - cc,9 >= 0
 (7): xa,0 + xb,0 + xc,0 <= 1
 (7): xa,1 + xb,1 + xc,1 <= 1
 (7): xa,2 + xb,2 + xc,2 <= 1
 (7): xa,3 + xb,3 + xc,3 <= 1
 (7): xa,4 + xb,4 + xc,4 <= 1
 (7): xa,5 + xb,5 + xc,5 <= 1
 (7): xa,6 + xb,6 + xc,6 <= 1
 (7): xa,7 + xb,7 + xc,7 <= 1
 (7): xa,8 + xb,8 + xc,8 <= 1
 (7): xa,9 + xb,9 + xc,9 <= 1
 (8): - xa,0 + xa,1 - xb,0 + xb,1 - xc,0 + xc,1 <= 0
 (8): - xa,1 + xa,2 - xb,1 + xb,2 - xc,1 + xc,2 <= 0
 (8): - xa,2 + xa,3 - xb,2 + xb,3 - xc,2 + xc,3 <= 0
 (8): - xa,3 + xa,4 - xb,3 + xb,4 - xc,3 + xc,4 <= 0
 (8): - xa,4 + xa,5 - xb,4 + xb,5 - xc,4 + xc,5 <= 0
 (8): - xa,5 + xa,6 - xb,5 + xb,6 - xc,5 + xc,6 <= 0
 (8): - xa,6 + xa,7 - xb,6 + xb,7 - xc,6 + xc,7 <= 0
 (8): - xa,7 + xa,8 - xb,7 + xb,8 - xc,7 + xc,8 <= 0
 (8): - xa,8 + xa,9 - xb,8 + xb,9 - xc,8 + xc,9 <= 0
 (9): xa,0 >= 0
 (9): xa,1 >= 0
 (9): xa,2 >= 0
 (9): xa,3 >= 0
 (9): xa,4 >= 0
 (9): xa,5 >= 0
 (9): xa,6 >= 0
 (9): xa,7 >= 0
 (9): xa,8 >= 0
 (9): xa,9 >= 0
 (9): xb,0 >= 0
 (9): xb,1 >= 0
 (9): xb,2 >= 0
 (9): xb,3 >= 0
 (9): xb,4 >= 0
 (9): xb,5 >= 0
 (9): xb,6 >= 0
 (9): xb,7 >= 0
 (9): xb,8 >= 0
 (9): xb,9 >= 0
 (9): xc,0 >= 0
 (9): xc,1 >= 0
 (9): xc,2 >= 0
 (9): xc,3 >= 0
 (9): xc,4 >= 0
 (9): xc,5 >= 0
 (9): xc,6 >= 0
 (9): xc,7 >= 0
 (9): xc,8 >= 0
 (9): xc,9 >= 0
 (6): [ xa,0 * ca,0 + xa,0 * ca,1 + xa,0 * ca,2 + xa,1 * ca,1 + xa,1 * ca,2
   + xa,1 * ca,3 + xa,2 * ca,2 + xa,2 * ca,3 + xa,2 * ca,4 + xa,3 * ca,3
   + xa,3 * ca,4 + xa,3 * ca,5 + xa,4 * ca,4 + xa,4 * ca,5 + xa,4 * ca,6
   + xa,5 * ca,5 + xa,5 * ca,6 + xa,5 * ca,7 + xa,6 * ca,6 + xa,6 * ca,7
   + xa,6 * ca,8 + xa,7 * ca,7 + xa,7 * ca,8 + xa,7 * ca,9 + xa,8 * ca,8
   + xa,8 * ca,9 + xa,9 * ca,9 ] >= 5
 (6): [ xb,0 * cb,0 + xb,0 * cb,1 + xb,0 * cb,2 + xb,1 * cb,1 + xb,1 * cb,2
   + xb,1 * cb,3 + xb,2 * cb,2 + xb,2 * cb,3 + xb,2 * cb,4 + xb,3 * cb,3
   + xb,3 * cb,4 + xb,3 * cb,5 + xb,4 * cb,4 + xb,4 * cb,5 + xb,4 * cb,6
   + xb,5 * cb,5 + xb,5 * cb,6 + xb,5 * cb,7 + xb,6 * cb,6 + xb,6 * cb,7
   + xb,6 * cb,8 + xb,7 * cb,7 + xb,7 * cb,8 + xb,7 * cb,9 + xb,8 * cb,8
   + xb,8 * cb,9 + xb,9 * cb,9 ] >= 7.07
 (6): [ xc,0 * cc,0 + xc,0 * cc,1 + xc,0 * cc,2 + xc,1 * cc,1 + xc,1 * cc,2
   + xc,1 * cc,3 + xc,2 * cc,2 + xc,2 * cc,3 + xc,2 * cc,4 + xc,3 * cc,3
   + xc,3 * cc,4 + xc,3 * cc,5 + xc,4 * cc,4 + xc,4 * cc,5 + xc,4 * cc,6
   + xc,5 * cc,5 + xc,5 * cc,6 + xc,5 * cc,7 + xc,6 * cc,6 + xc,6 * cc,7
   + xc,6 * cc,8 + xc,7 * cc,7 + xc,7 * cc,8 + xc,7 * cc,9 + xc,8 * cc,8
   + xc,8 * cc,9 + xc,9 * cc,9 ] >= 10.19
Bounds
Binaries
 xa,0 xa,1 xa,2 xa,3 xa,4 xa,5 xa,6 xa,7 xa,8 xa,9 xb,0 xb,1 xb,2 xb,3 xb,4
 xb,5 xb,6 xb,7 xb,8 xb,9 xc,0 xc,1 xc,2 xc,3 xc,4 xc,5 xc,6 xc,7 xc,8 xc,9
Generals
 ca,0 ca,1 ca,2 ca,3 ca,4 ca,5 ca,6 ca,7 ca,8 ca,9 cb,0 cb,1 cb,2 cb,3 cb,4
 cb,5 cb,6 cb,7 cb,8 cb,9 cc,0 cc,1 cc,2 cc,3 cc,4 cc,5 cc,6 cc,7 cc,8 cc,9
End
