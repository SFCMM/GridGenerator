//
// Created by svenb on 7/12/21.
//

#ifndef GRIDGENERATOR_GRIDGENERATOR_H
#define GRIDGENERATOR_GRIDGENERATOR_H

template <int DEBUG_LEVEL>
class GridGenerator {
 public:
  GridGenerator()  = default;
  ~GridGenerator() = default;

  auto run(int argc, char** argv) -> int;

 private:
};


#endif // GRIDGENERATOR_GRIDGENERATOR_H
