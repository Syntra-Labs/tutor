package methodology;

import java.util.List;

class Scenario {
    private final String description;
    private final List<Hypothesis> hypotheses;
    private final String hiddenTruth;
    
    public Scenario(String description, String hiddenTruth, List<Hypothesis> hypotheses) {
        this.description = description;
        this.hiddenTruth = hiddenTruth;
        this.hypotheses = hypotheses;
    }
    
    public List<Hypothesis> getHypotheses() { return hypotheses; }
    public String getHiddenTruth() { return hiddenTruth; }
}


