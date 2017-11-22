#include <dynfu/utils/solver.hpp>

#define KNN 8

Solver::Solver(ceres::Solver::options options) { this.options = options; }

Solver::~Solver() {}

static ceres::CostFunction* Create(std::shared_ptr<Warpfield> warpfield, std::shared_ptr<Frame> canonicalFrame,
                                   std::shared_ptr<Frame> liveFrame) {
    auto costFunctor  = new CostFunctor(warpfield, canonicalFrame, liveFrame);
    auto costFunction = new ceres::DynamicAutoDiffCostFunction<CostFunctor, 6>(costFunctor);

    int numResiduals = 3;
    int noParameters = KNN * 6;

    for (int i = 0; i < KNN; i++) {
        costFunction->AddParameterBlock(noParameters);
    }

    costFunction->SetNumResiduals(numResiduals);

    return costFunction;
}

void calculateWarpToLive(std::shared_ptr<Warpfield> warpfield, std::shared_ptr<Frame> canonicalFrame,
                         std::shared_ptr<Frame> liveFrame) {
    ceres::CostFunction* costFunction = this->Create(warpfield, canonicalFrame, liveFrame);

    ceres::Problem problem;
    problem.AddResidualBlock(costFunction, NULL);

    ceres::Solver::Summary summary;

    ceres::Solve(this.options, &problem, &summary);

    std::cout << summary.FullReport() << "\n";
}