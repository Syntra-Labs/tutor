package methodology;

import java.util.*;
import java.util.stream.Collectors;

// Core classes
class Hypothesis {
    private final String name;
    private double prior;
    private double likelihood;
    
    public Hypothesis(String name, double prior) {
        this.name = name;
        this.prior = prior;
        this.likelihood = 1.0;
    }
    
    // Getters and setters
    public void updateLikelihood(double multiplier) {
        this.likelihood *= multiplier;
    }
    
    public double getPosterior(double totalEvidence) {
        return (prior * likelihood) / totalEvidence;
    }
    
    public double getLikelihood() {return likelihood;}
    public String getName() { return name; }
    public double getPrior() { return prior; }
    public void setPrior(double prior) { this.prior = prior; }
}
