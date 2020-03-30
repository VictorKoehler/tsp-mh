#include <vector>

/**
 * Keep track of how effective pertubations movements were using the ratio
 * between common edges.
 * Identical solutions equals to 1, completely different to 0.
 */
class CommonEdgeDifferenceMetric {
   private:
    bool keepHistory;

   public:
    double sum;
    unsigned int numInterations, numClients, onesSum;
    std::vector<int> previousSolution;
    std::vector<double> history;

    CommonEdgeDifferenceMetric(unsigned int numClients=0, bool keepHistory=false)
        : keepHistory(keepHistory), sum(0), numInterations(0), numClients(numClients), onesSum(0) {}

    /**
     * Stack up a solution represented by many routes separated by 0.
     * Return the current difference between flattenRoutesSolution and the previous solution.
     * Identical solutions equals to 1, completely different to 0.
     * If there is no previous solution, return 0.
     * Assumes flattenRoutesSolution[0] = flattenRoutesSolution[flattenRoutesSolution.size()-1] = 0
     */
    double stackSolution(std::vector<int>& flattenRoutesSolution);

    /**
     * Reset previous solution but keep sum, numInterations, ones and history.
     * This is useful when used between first level iterations of ILS, when a
     * new solution is constructed and not pertubed.
     */
    void unstack();

    /**
     * Return the average of ratio between all solutions evaluated.
     */
    double average();

    /**
     * Return the ratio of 1's against the total number of evaluations.
     * That means how many percents of solutions were equal.
     */
    double ones_ratio();

    /**
     * Print results friendly.
     */
    void printResults();
};