velocity = linspace(3, 12, 100);
deltaS = 0.5;
deltaT = deltaS./velocity;
uT = (deltaS./(velocity - 0.5)) - (deltaS./(velocity + 0.5));
plot(velocity, uT);
