# Makefile for Dhaka City Routing Solver - All 6 Problems

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LDFLAGS = -lm

# Target executables (descriptive names)
P1 = P1_shortest_car_route
P2 = P2_cheapest_car_metro
P3 = P3_cheapest_all_modes
P4 = P4_cheapest_with_schedules
P5 = P5_fastest_with_schedules
P6 = P6_cheapest_with_deadline

TARGETS = $(P1) $(P2) $(P3) $(P4) $(P5) $(P6)

# Header dependencies
HEADERS = graph_utils.h graph_loader.h

# Default target - build all
all: $(TARGETS)
	@echo "✓ All 6 problems compiled successfully!"
	@echo ""
	@echo "Executables:"
	@for exe in $(TARGETS); do ls -lh $$exe 2>/dev/null | awk '{print "  " $$9 " (" $$5 ")"}'; done

# Individual problem targets
$(P1): P1_shortest_car_route.cpp $(HEADERS)
	@echo "Compiling Problem 1 (Shortest car route)..."
	$(CXX) $(CXXFLAGS) -o $(P1) P1_shortest_car_route.cpp $(LDFLAGS)

$(P2): P2_cheapest_car_metro.cpp $(HEADERS)
	@echo "Compiling Problem 2 (Cheapest car + metro)..."
	$(CXX) $(CXXFLAGS) -o $(P2) P2_cheapest_car_metro.cpp $(LDFLAGS)

$(P3): P3_cheapest_all_modes.cpp $(HEADERS)
	@echo "Compiling Problem 3 (Cheapest all modes)..."
	$(CXX) $(CXXFLAGS) -o $(P3) P3_cheapest_all_modes.cpp $(LDFLAGS)

$(P4): P4_cheapest_with_schedules.cpp $(HEADERS)
	@echo "Compiling Problem 4 (Cheapest with schedules)..."
	$(CXX) $(CXXFLAGS) -o $(P4) P4_cheapest_with_schedules.cpp $(LDFLAGS)

$(P5): P5_fastest_with_schedules.cpp $(HEADERS)
	@echo "Compiling Problem 5 (Fastest with schedules)..."
	$(CXX) $(CXXFLAGS) -o $(P5) P5_fastest_with_schedules.cpp $(LDFLAGS)

$(P6): P6_cheapest_with_deadline.cpp $(HEADERS)
	@echo "Compiling Problem 6 (Cheapest with deadline)..."
	$(CXX) $(CXXFLAGS) -o $(P6) P6_cheapest_with_deadline.cpp $(LDFLAGS)

# Test targets
test1: $(P1)
	@echo "Testing Problem 1..."
	@printf "90.404772 23.855136\n90.363501 23.805117\n" | ./$(P1)

test2: $(P2)
	@echo "Testing Problem 2..."
	@printf "90.404772 23.855136\n90.363501 23.805117\n" | ./$(P2)

test3: $(P3)
	@echo "Testing Problem 3..."
	@printf "90.404772 23.855136\n90.363501 23.805117\n" | ./$(P3)

test4: $(P4)
	@echo "Testing Problem 4..."
	@printf "90.404772 23.855136\n90.363501 23.805117\n10 30\n" | ./$(P4)

test5: $(P5)
	@echo "Testing Problem 5..."
	@printf "90.404772 23.855136\n90.363501 23.805117\n10 30\n" | ./$(P5)

test6: $(P6)
	@echo "Testing Problem 6..."
	@printf "90.404772 23.855136\n90.363501 23.805117\n10 30\n18 0\n" | ./$(P6)

# Clean targets
clean:
	@echo "Cleaning build files..."
	rm -f $(TARGETS)
	rm -f *.o
	@echo "✓ Clean complete"

cleanall: clean
	@echo "Cleaning KML files..."
	rm -f P1_route.kml P2_route.kml P3_route.kml P4_route.kml P5_route.kml P6_route.kml
	rm -f problem*.kml
	@echo "✓ All generated files removed"

# Help target
help:
	@echo "Dhaka City Routing Solver - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build all 6 problems"
	@echo "  make $(P1) - Build Problem 1 only"
	@echo "  make $(P2) - Build Problem 2 only"
	@echo "  make $(P3) - Build Problem 3 only"
	@echo "  make $(P4) - Build Problem 4 only"
	@echo "  make $(P5) - Build Problem 5 only"
	@echo "  make $(P6) - Build Problem 6 only"
	@echo "  make test1-6  - Test individual problems"
	@echo "  make clean    - Remove executables"
	@echo "  make cleanall - Remove executables and KML files"
	@echo "  make help     - Show this help"
	@echo ""
	@echo "Problem descriptions:"
	@echo "  P1: Shortest car route (distance)"
	@echo "  P2: Cheapest route (car + metro)"
	@echo "  P3: Cheapest route (all modes)"
	@echo "  P4: Cheapest route with schedules"
	@echo "  P5: Fastest route with schedules"
	@echo "  P6: Cheapest route with time constraints"

.PHONY: all clean cleanall help test1 test2 test3 test4 test5 test6
