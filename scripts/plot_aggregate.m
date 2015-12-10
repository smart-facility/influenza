clc
close all
clear

Data = dlmread('../output/sim_out_20.csv',';',1,0);
Data(:,1) = Data(:,1) / 3600;

plot(Data(:,1),Data(:,2))
hold all
plot(Data(:,1),Data(:,3))
plot(Data(:,1),Data(:,4))
plot(Data(:,1),Data(:,5))
plot(Data(:,1),Data(:,6))

legend('susceptible','latent','asymptomatic','symptomatic','recovered')
title('r.beta = 1, beta = 0.001, epsilon.inv = 0.52, p.a = 0.33, mu.inv = 0.33, max.inf = 20')
