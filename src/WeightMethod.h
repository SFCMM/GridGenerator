#ifndef GRIDGENERATOR_WEIGHTMETHOD_H
#define GRIDGENERATOR_WEIGHTMETHOD_H

enum class WeightMethodsTypes { uniform };

class WeightMethod {
  virtual auto weight(GInt id) -> GFloat = 0;
};


class WeightUniform : public WeightMethod {
  auto weight(GInt /*id*/) -> GFloat override { return 1.0; }
};

#endif // GRIDGENERATOR_WEIGHTMETHOD_H
