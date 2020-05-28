#!/usr/bin/python3

"""
@brief : construct a python dictionary from csv data if the protein is the location store true else false
@:param: csv_file file to build from
@:param: dict to create
@:param: location_id location id to store
@:return: void
         dict contain tuple (string,boolean)
"""
def construct_dict(csv_file, dict, location_id):
    with open(csv_file) as f:
        line = iter(f)
        line = next(line)
        for line in f:

            if len(line) > 1:
                (key, value) = line.split(',', 1)
                value = value.replace('[', '').replace(']', '').replace('"', '')
                value = value.split(',')
                map_object = map(int, value)
                tmp = False
                for a in list(map_object):
                    if a == location_id:
                        tmp = True
                dict[key] = tmp


"""
@brief : calculate positive and negative rate from a file and a dictionary
@:param: file containing protein name found by the individual
@:param: d is a dictionary to know if the protein is really true or not
@:returns : number of truePositive, falsePositive, trueNegative, falseNegative
"""
def predict_conditions(file, d):
    true_positive = 0
    false_positive = 0
    true_negative = 0
    false_negative = 0
    result = open(file, "r")
    positive = {}
    negative = d.copy()

    for line in result:
        elements = line.split('\t')

        if elements[0] in negative.keys():
            if d[elements[0]]:
                true_positive += 1
                positive[elements[0]] = True
            else:
                false_positive += 1
                positive[elements[0]] = False
            del negative[elements[0]]

    for neg in negative.values():
        if neg:
            false_negative += 1
        else:
            true_negative += 1

    return true_positive, false_positive, true_negative, false_negative



"""
@brief : calculate ppv score
@:param : TP true positive
@:param : FP false positive
@:return ppv score 
"""
def ppv(TP, FP):
    return TP / (FP + TP)

"""
@brief : calculate tpr score
@:param : TP true positive
@:param : P  positive
@:return tpr score 
"""
def tpr(TP, P):
    return TP / P

"""
@brief : calculate f1 score
@:param : file  file containing protein name found by the individual
@:param : d is a dictionary to know if the protein is really true or not
@:return f1 score 
"""
def f1(file, d):
    TP, FP, TN, FN = predict_conditions(file, d)
    ppv_value = ppv(TP, FP)
    tpr_value = tpr(TP, TP + FN)
    if ppv_value + tpr_value == 0:
        return 0
    return 2 * ((ppv_value * tpr_value) / (ppv_value + tpr_value))
