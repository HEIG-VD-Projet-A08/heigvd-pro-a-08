#!/usr/bin/python3

import os
import random
from random import randint
from deap import base
from deap import creator
from deap import tools
import time
from server import client_data_channel, client_commands_thread
import predictionTools
from datetime import datetime
import math
import threading
import globals

toolbox = base.Toolbox()

PROTEIN_ALPHABET = ['A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'Y']
WEIGHT_ACCURACY = 5
WEIGHT_COVERAGE = 5
WEIGHT_DIFFERENT_ELEM = 1
WEIGHT_TEST = 10
WEIGHT_PREDICT = 10
WEIGHT_FITNESSES = [WEIGHT_ACCURACY, WEIGHT_COVERAGE, WEIGHT_DIFFERENT_ELEM, WEIGHT_TEST, WEIGHT_PREDICT]
USE_AS_SERVER = True

individual_size = 10
population_size = 100
e_value = 100
best_individual_fitness = 0
number_iteration = 0
iter_max = 500
min_size_word = 10
max_size_word = 30
size_word = [min_size_word, max_size_word]
location_id = 0
d = {}
best_predict = 0
best_f1 = 0
date_experience = datetime.now().strftime("%Y-%m-%d-%H:%M:%S")

first_opening = False
client_socket = ""


"""
@brief : setup the deap toolbox
@:param size_word array of minimum and max size of word to generate
"""
def setup_toolbox(size_word):
    global toolbox
    toolbox.register("generateRandomTexte", generate_text, size_word[0], size_word[1])
    toolbox.register("individual", tools.initRepeat, creator.Individual,
                     toolbox.generateRandomTexte, individual_size)

    # define the population to be a list of individuals
    toolbox.register("population", tools.initRepeat, list, toolbox.individual)
    # ----------
    # Operator registration
    # ----------
    # register the goal / fitness function
    toolbox.register("evaluate", evaluate)

    # register the crossover operator
    toolbox.register("mate", tools.cxTwoPoint)

    # register a mutation operator with a probability to
    # flip each attribute/gene of 0.05
    toolbox.register("mutate", tools.mutFlipBit, indpb=0.1)

    # operator for selecting individuals for breeding the next
    # generation: each individual of the current generation
    # is replaced by the 'fittest' (best) of three individuals
    # drawn randomly from the current generation.
    toolbox.register("select", tools.selTournament, tournsize=3)


# DEAP initialisation
creator.create("FitnessMax", base.Fitness, weights=(1,))
creator.create("Individual", list, fitness=creator.FitnessMax)


"""
@brief : generate random text in the protein alphabet
@:param nbr_char_min : minimum size of the string
@:param nbr_char_max : maximum size 
@:return string
"""
def generate_text(nbr_char_min, nbr_char_max):
    size = randint(nbr_char_min, nbr_char_max)
    string = ""
    for i in range(0, size):
        string += PROTEIN_ALPHABET[randint(0, len(PROTEIN_ALPHABET) - 1)]
    return string


"""
@brief : calculate prediction with blast and return a f1 score
@:return : f1 score
"""
def predict():
    path_dict = "topredict.csv"
    path_out_blast ="/tmp/topredict"
    os.system("./blastp  -db db/topredict.fasta -query /tmp/toBlast.fasta -out " + path_out_blast +" -evalue " + str(e_value) + " -word_size 2 -gapopen 10 -gapextend 1  -task blastp -outfmt '6 saccver  qacc ppos qseq sseq qcovs' -subject_besthit")
    if os.path.getsize(path_dict) != 0:
        to_predict = {}

        predictionTools.construct_dict(path_dict, to_predict, location_id)
        return predictionTools.f1(path_out_blast, to_predict)

    return 0


"""
@brief : generate a file "toBlast.fasta in the directory /tmp/
         this file contains different word from
@:param : individual is one individual from the whole population

"""
def get_different_indivudal(individual):
    different_word_path = "/tmp/toBlast.fasta"
    out_blast_path = "/tmp/resultSim"
    different_word_file = open(different_word_path, "w")
    individual_list = iter(individual)
    first_word = next(individual_list)

    # set the first word in the file
    different_word_file.write(">" + first_word + "\n" + first_word + "\n")
    different_word_file.close()

    for word in individual_list:
        #create a file with a word to blast over the file "toBlast.fasta"
        tmp_file = open("/tmp/tmp.fasta","w")
        tmp_file.write(">" + word + "\n" + word + "\n")
        tmp_file.close()

        os.system("./blastp  -subject /tmp/tmp.fasta -query " + different_word_path + " -out " + out_blast_path + \
                  " -evalue 0.1 -word_size 2 -gapopen 10 -gapextend 1  -task blastp -outfmt '6 saccver  qacc ppos qseq sseq qcovs' -subject_besthit")

        result = open(out_blast_path, "r")
        nbr_elem = 0
        # add the word to the file if is not found by blast or if the accuracy is lower than 90
        for line in result:
            elements = line.split('\t')
            nbr_elem += 1
            if float(elements[2]) < 90:
                different_word_file = open(different_word_path, "a")
                different_word_file.write(">" + word + "\n" + word + "\n")
                different_word_file.close()
        result.close()

        if nbr_elem == 0:
            different_word_file = open(different_word_path, "a")
            different_word_file.write(">" + word + "\n" + word + "\n")
            different_word_file.close()
    os.system("rm /tmp/tmp.fasta")


"""
@brief : calculate a fitness value weighted by multi factor
            5 fitness are set :
                accuracy        : average of similarity between word and protein subsequence selected by blastp
                coverage        : average of rate between length of the word and the subsequence selected by blastp
                nbrElement      : number of word used in the indivual
                test f1 score   : f1 score calculate from the "learning database"
                predict f1 score: f1 score calculate from the "prediction database"
            Fitnesses are weighted by the constant WEIGHT_FITNESSES
@:param : individual is a list of word to blast over blastp
@:return: weighted sum of fitnesses 
"""
def evaluate(individual):
    global number_iteration, best_individual_fitness, best_predict, best_f1, e_value, WEIGHT_FITNESSES
    get_different_indivudal(individual)
    # file containing the blast_result
    path_blast = "/tmp/result"
    os.system("./blastp -db db/small.fasta -query /tmp/toBlast.fasta -out " + path_blast + " -evalue " + str(e_value) + " -word_size 2 -gapopen 10 -gapextend 1 -num_threads 2 -task blastp -outfmt '6 saccver  qacc ppos qseq sseq length' -subject_besthit")

    accuracy = 0
    nbr_elem = 0
    nbr_different_string = []
    coverage_rate = 0
    f1_val = 0
    predict_val = 0
    sum_of_fitnesses = 0
    if os.path.getsize(path_blast) != 0:
        blast_result = open(path_blast, "r")
        for line in blast_result:
            elements = line.split('\t')
            accuracy += float(elements[2]) *0.01

            # a longer word is preferable
            coverage_rate += float(elements[5])/len(elements[1]) * (len(elements[1]) / size_word[0])
            # count number of line to calculate average
            nbr_elem += 1

            # add word if is not in the array
            if elements[1] not in nbr_different_string:
                nbr_different_string.append(elements[1])

        blast_result.close()
        # f1 score calculate from the "learning database"
        f1_val = predictionTools.f1(path_blast,d)

        # f1 score calculate from the "prediction database"
        predict_val = predict()

        sum_of_fitnesses = (WEIGHT_FITNESSES[0]) * (accuracy / nbr_elem) \
                         + WEIGHT_FITNESSES[1] * (coverage_rate / nbr_elem) \
                         + WEIGHT_FITNESSES[2] * len(nbr_different_string) \
                         + WEIGHT_FITNESSES[3] * f1_val \
                         + WEIGHT_FITNESSES[4] * predict_val
    # store the best individual in the directory  small and the date of the experience
    if sum_of_fitnesses > best_individual_fitness:
        nb_digit = "{:0>" + str(math.floor(math.log10(iter_max)) + 1) + "d}"
        os.system("cp /tmp/result small/" + date_experience + str("_it") + str(nb_digit.format(number_iteration)))
        best_individual_fitness = sum_of_fitnesses
        # store in global value for the best individual f1 score
        best_predict = predict_val
        best_f1 = f1_val

    os.remove(path_blast)
    return sum_of_fitnesses,


"""
@brief : send data to the gui
@:param : ind, contains the words generated for this iteration
@:param : it, value of the current iteration
@:param : predict, f1 score of the "prediction database"
@:param : test, f1 score of the "learning database"
"""
def send_data(ind, it, predict, test):
    global client_socket, first_opening
    s = ""
    if first_opening:
        s = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        first_opening = False

    s = s + """
            <result>
                <it> %s </it>
                <predict> %s </predict>
                <test> %s </test>
                
            """ % (it, predict, test)
    for word in ind:
        s += """<word> %s </word>""" % word
    s += "</result>"
    client_socket.send(s.encode("utf-8"))
    print(s + " has been sent")


def main():
    global client_socket, iter_max, size_word, best_f1, location_id, number_iteration, e_value, \
        best_individual_fitness, best_predict, first_opening, individual_size
    first_opening = True
    # CXPB  is the probability with which two individuals
    #       are crossed
    #
    # MUTPB is the probability for mutating an individual
    CXPB, MUTPB = 0.8, 0.1

    if USE_AS_SERVER:
        globals.stopReceived = False
        globals.stopRequested = False
        globals.shouldListen = True
        size_word, individual_size, iter_max, client_socket = client_data_channel()
        thread = ProcessCommands(client_socket)
        thread.start()
    pathDict = "output.csv"
    # Create dictionary for the database and the location
    predictionTools.construct_dict(pathDict, d, location_id)
    setup_toolbox(size_word)
    number_iteration = 0

    print("Start of evolution")
    pop = toolbox.population(n=population_size)
    # Evaluate the entire population
    fitnesses = list(map(toolbox.evaluate,pop))
    for ind, fit in zip(pop, fitnesses):
        ind.fitness.values = fit

    # Begin the evolution
    while number_iteration < iter_max and not globals.stopReceived:
        start = time.time()
        # reset the best individual fitness for the current iteration
        best_individual_fitness = 0
        number_iteration = number_iteration + 1
        print("-- Generation %i --" % number_iteration)

        # Select the next generation individuals
        offspring = toolbox.select(pop, len(pop))
        # Clone the selected individuals
        offspring = list(map(toolbox.clone, offspring))

        # Apply crossover and mutation on the offspring
        for child1, child2 in zip(offspring[::2], offspring[1::2]):

            # cross two individuals with probability CXPB
            if random.random() < CXPB:
                toolbox.mate(child1, child2)

                # fitness values of the children
                # must be recalculated later
                del child1.fitness.values
                del child2.fitness.values

        for mutant in offspring:
            # mutate an individual with probability MUTPB
            if random.random() < MUTPB:
                toolbox.mutate(mutant)
            del mutant.fitness.values

        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in offspring if not ind.fitness.valid]
        fitnesses = map(toolbox.evaluate, invalid_ind)
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit

        print("  Evaluated %i individuals" % len(invalid_ind))

        # The population is entirely replaced by the offspring
        pop[:] = offspring

        # Gather all the fitnesses in one list and print the stats
        fits = [ind.fitness.values[0] for ind in pop]
        ind1 = pop[0]
        for ind in pop:
            if ind1.fitness.values[0] < ind.fitness.values[0]:
                ind1 = ind

        print("  Min %s" % min(fits))
        print("  MAX %s" % max(fits))
        print("  predict %s" % (best_predict * 100))
        print("  test %s" % (best_f1 * 100))
        print("evalue %0.10f" % e_value)
        print("time %f" %(time.time() - start))
        if USE_AS_SERVER and not globals.stopReceived:
            send_data(ind1, number_iteration, best_predict * 100, best_f1 * 100)
            if globals.stopRequested:
                break

    print("-- End of (successful) evolution --")

    best_ind = tools.selBest(pop, 1)[0]
    worst_ind = tools.selWorst(pop,1)[0]
    print("Best individual is %s, %s\n" % (best_ind, best_ind.fitness.values))
    print("worst individual is %s, %s" % (worst_ind, worst_ind.fitness.values))

    if USE_AS_SERVER:
        globals.shouldListen = False
        thread.join()


# FROM: https://www.tutorialspoint.com/python3/python_multithreading.htm
class ProcessCommands (threading.Thread):
    def __init__(self, cs):
        threading.Thread.__init__(self)
        self.cs = cs

    def run(self):
        client_commands_thread(self.cs)


if __name__ == "__main__":
    if USE_AS_SERVER:
        while True:
            main()
    else:
        main()
