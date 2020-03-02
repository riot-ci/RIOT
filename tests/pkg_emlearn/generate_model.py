#!/usr/bin/env python3

import emlearn
import joblib

estimator = joblib.load("model")
cmodel = emlearn.convert(estimator, method='inline')
cmodel.save(file='sonar.h')
