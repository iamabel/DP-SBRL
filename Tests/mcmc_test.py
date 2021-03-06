#!/usr/bin/env python
import sys
sys.path.append("../BRL")
from data import *
from mcmc import *

LOOP_ITERATIONS = 500

# If we do not run any MCMC iterations we should get a rule list consisting of all possible rules with
# the corresponding information correct (ie. label, rules, captures, unused)
def initRLTest():
    all_rules_and_captures = run("../Data/fim_1.txt", "../Data/dat2_test.txt", "diapers", 5.0, 3.0, 0)
    all_rules = [["beer"], ["cola"], ["bread","milk"], ["beer","milk"], ["milk"], ["cola","milk"], ["beer","bread"], ["bread"]]
    captures = [[0,3], [1,1]]
    all_passed = True

    all_passed = all_passed and (all_rules_and_captures.rules == [all_rules[0]])
    all_passed = all_passed and (all_rules_and_captures.label == "diapers")
    all_passed = all_passed and (all_rules_and_captures.captures == captures)
    all_passed = all_passed and (all_rules_and_captures.unused == all_rules[1:])

    return all_passed
def mathmaticalTests():
    all_passed = True       # Allows us to run all tests without quitting at a failure.
    # Test likelihood
    N = [[1,1], [2,3], [0,0], [4,9]]
    output_likelihood = likelihood(N)
    ex_likelihood = ((1.0/6.0) * (12.0/720.0) * 1.0 * (8709120.0/87178291200.0))
    if (output_likelihood - np.log(ex_likelihood)) >= .0000001:
        all_passed = False
        print("likelihood function failed, expect: " + str(np.log(ex_likelihood)) + " got: " + str(output_likelihood))

    # Test rules_list_length
    output_list_length = rules_list_length(4, 2, 3)
    ex_list_length = ((81.0)/24.0)
    if (output_list_length - np.log(ex_list_length)) >= .0000001:
        all_passed = False
        print("rules_list_length function failed, expect: " + str(np.log(ex_list_length)) + " got: " + str(output_list_length))

    # Test antecedent_length
    output_antecedent_length = antecedent_length(3, [['hi', 'there'],['i', 'am', 'here'], ['this', 'has', 'five', 'words', 'now']], 2)
    ex_antecedent_length = ((8.0)/6.0) / (2.0 + 8.0/6.0 + 32.0/120.0)
    if (output_antecedent_length - np.log(ex_antecedent_length)) >= .0000001:
        all_passed = False
        print("antecedent_length function failed, expect: " + str(np.log(ex_antecedent_length)) + " got: " + str(output_antecedent_length))

    # Test prior
    output_prior = prior(run("../Data/dummy_fim.txt", "../Data/dummy_data.txt", "x", 3.0, 2.0, 0), 3, 2)
    ex_prior = 3.0
    if (output_prior - np.log(ex_prior)) >= .0000001:
        all_passed = False
        print("prior function failed, expect: " + str(np.log(ex_prior)) + " got: " + str(output_prior))

    return all_passed
def skewedMCMC():
    d1 = run("../Data/skewed_fim.txt", "../Data/skewed_data.txt", "hi", 1.0, 1.0, 100000)
    d2 = run("../Data/skewed_fim.txt", "../Data/skewed_data.txt", "hi", 1.0, 1.0, 100000)
    return d1.rules == d2.rules

# Note short_fim.txt has only 3 antecedents which means there are 18 potential rule lists. These were
# made from dat2_test.txt with minsup=.8
def sampleEnd():
    actual_rl_d = {}
    all_d = []
    d = RuleList("../Data/short_fim.txt", "../Data/dat2_test.txt",  "diapers")

    for _ in range(LOOP_ITERATIONS//2):
        for _ in range(LOOP_ITERATIONS):
            d = mcmc_mh(d, 3.0, 1.0)
        hashable_d = d.strNeat()
        all_d.append(d)
        if hashable_d not in actual_rl_d:
            actual_rl_d[hashable_d] = 1
        else:
            actual_rl_d[hashable_d] += 1

    normalized_actual = {rl:occ/(LOOP_ITERATIONS//2) for rl, occ in actual_rl_d.items()}
    expected_rl_d = {rl:np.exp(score(rl)) for rl in all_d}
    # TODO(iamabel): Graph expected against actual or find a way to tell their difference.

def main():
    print("init Rule List built correctly? " + str(initRLTest()))
    print("All Mathematical Tests Passed? " + str(mathmaticalTests()))
    print("Did the MCMC work? " + str(skewedMCMC()))

main()
