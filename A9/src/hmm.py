''' Now  consider  an  Order-2  HMM  where  Z(t)  depends  on  both  Z(t-1)  and  Z(t-2),  as 
p(Z(t)=k|Z(t-1)=j,Z(t-2)=i)  α  1/(|k-i|+|k-j|1).  All  states  are  equally  likely  at  t=1  and  t=2. 
Consider  an  observation  sequence X  of  length  T. You  want  to estimate  the  possible 
values  of  the  corresponding  latent  states  using  Gibbs  Sampling.  Provide  the  full 
algorithm, including the sampling steps for all variables, and how you will estimate the 
marginal distributions of each latent variable from the collected samples
'''

import numpy as np
import matplotlib.pyplot as plt
import random
import math

# Function to generate the transition matrix
def generate_transition_matrix(order):
    transition_matrix = np.zeros((order, order))
    for i in range(order):
        for j in range(order):
            if i != j:
                transition_matrix[i][j] = 1/(abs(i-j)+1)
    return transition_matrix

# Function to generate the emission matrix

def generate_emission_matrix(order, num_states):
    emission_matrix = np.zeros((order, num_states))
    for i in range(order):
        for j in range(num_states):
            emission_matrix[i][j] = 1/num_states
    return emission_matrix

# Function to generate the initial state distribution
def generate_initial_state_distribution(order):
    initial_state_distribution = np.zeros(order)
    for i in range(order):
        initial_state_distribution[i] = 1/order
    return initial_state_distribution

# Function to generate the observation sequence
def generate_observation_sequence(num_states, num_observations):
    observation_sequence = np.zeros(num_observations)
    for i in range(num_observations):
        observation_sequence[i] = random.randint(0, num_states-1)
    return observation_sequence

# Function to generate the latent state sequence
def generate_latent_state_sequence(order, num_states, num_observations):
    latent_state_sequence = np.zeros(num_observations)
    for i in range(num_observations):
        latent_state_sequence[i] = random.randint(0, num_states-1)
    return latent_state_sequence



# transition_matrix is already generated
# emission_matrix is already generated
# initial_state_distribution is already generated
# observation_sequence is already generated
# latent_state_sequence is already generated
# write the code after that

# write code for gibbs sampling by initializing the latent state sequence with random values
# and then sampling the latent state sequence using the transition matrix and emission matrix
# and observation sequence

# Function to sample the latent state sequence
def sample_latent_state_sequence(order, num_states, num_observations, transition_matrix, emission_matrix, observation_sequence):
    latent_state_sequence = np.zeros(num_observations)
    for i in range(num_observations):
        if i == 0:
            latent_state_sequence[i] = random.randint(0, num_states-1)
        elif i == 1:
            latent_state_sequence[i] = random.randint(0, num_states-1)
        else:
            latent_state_sequence[i] = random.randint(0, num_states-1)
    return latent_state_sequence


# call all the functions to generate the required matrices and sequences
order = 3
num_states = 4
num_observations = 10
transition_matrix = generate_transition_matrix(order)
emission_matrix = generate_emission_matrix(order, num_states)
initial_state_distribution = generate_initial_state_distribution(order)
observation_sequence = generate_observation_sequence(num_states, num_observations)
# write arguments such that the latent state function above is compatible with the gibbs sampling function
latent_state_sequence = sample_latent_state_sequence(order, num_states, num_observations, transition_matrix, emission_matrix, observation_sequence)


# take burn in iterations as 50
# consider values after 10 iterations
# take 1000 samples

# Function to perform gibbs sampling with the given function as condition
# p(Z(t)=k|Z(t-1)=j,Z(t-2)=i)  α  1/(|k-i|+|k-j|1)
def gibbs_sampling(order, num_states, num_observations, transition_matrix, emission_matrix, observation_sequence, latent_state_sequence):
    latent_state_sequence = sample_latent_state_sequence(order, num_states, num_observations, transition_matrix, emission_matrix, observation_sequence)
    for i in range(50):
        for j in range(2, num_observations):
            latent_state_sequence[j] = random.randint(0, num_states-1)
            numerator = 1/(abs(latent_state_sequence[j]-latent_state_sequence[j-1])+1)
            denominator = 0
            for k in range(num_states):
                denominator += 1/(abs(k-latent_state_sequence[j-1])+1)
            latent_state_sequence[j] = numerator/denominator
    return latent_state_sequence

latent_state_sequence = gibbs_sampling(order, num_states, num_observations, transition_matrix, emission_matrix, observation_sequence, latent_state_sequence)
print(latent_state_sequence)

# Function to estimate the marginal distribution of each latent variable from the collected samples
def estimate_marginal_distribution(order, num_states, num_observations, transition_matrix, emission_matrix, observation_sequence, latent_state_sequence):
    marginal_distribution = np.zeros((num_observations, num_states))
    for i in range(num_observations):
        for j in range(num_states):
            # write without using count function
            count = 0
            for k in range(num_observations):
                if latent_state_sequence[k] == j:
                    count += 1
            marginal_distribution[i][j] = count/num_observations
    return marginal_distribution

marginal_distribution = estimate_marginal_distribution(order, num_states, num_observations, transition_matrix, emission_matrix, observation_sequence, latent_state_sequence)
print(marginal_distribution)









