#!/usr/bin/env python

import sys
from itertools import izip

def main():
  diff = 0
  joint_count = 0
  # Iterate over both files simulaneously
  with open(sys.argv[1]) as ground_truth, open(sys.argv[2]) as predictions:
    for gt, p in izip(ground_truth, predictions):
      joint_count += 1
      gt = gt.strip().split()
      p = p.strip().split()

      d = 0
#      print "Prediction: "
#      print p
#      print "Ground truth: "
#      print gt
      d += abs(float(gt[1]) - float(p[1]))**2
      for gt_val, p_val in zip(gt[2:], p[2:]):
#        print "ground truth: %s, prediction: %s" % (gt_val, p_val)
        d += abs(float(gt_val) - float(p_val))**2
#      print "diff^2: %f, diff: %f" % (d, d**(1/2.0))
#      print "Diff: "
      d = d**(1/2.0)
#      print d
      diff += d
#      print "Total diff: "
#      print diff
  diff /= joint_count
  print diff

main()
