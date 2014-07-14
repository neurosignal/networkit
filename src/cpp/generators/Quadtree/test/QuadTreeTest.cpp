/*
 * QuadTreeTest.cpp
 *
 *  Created on: 28.05.2014
 *      Author: Moritz v. Looz (moritz.looz-corswarem@kit.edu)
 */

#include "QuadTreeTest.h"
#include "../../../auxiliary/Random.h"
#include "../../../auxiliary/Log.h"
#include "../../HyperbolicSpace.h"

namespace NetworKit {

QuadTreeTest::QuadTreeTest() {
	// TODO Auto-generated constructor stub

}

QuadTreeTest::~QuadTreeTest() {
	// TODO Auto-generated destructor stub
}

TEST_F(QuadTreeTest, testQuadTreeInsertion) {
	count n = 1000;
	double R = 1;
	vector<double> angles(n);
	vector<double> radii(n);
	HyperbolicSpace::fillPoints(&angles, &radii, 1, 1);
	double max = 0;
	for (index i = 0; i < n; i++) {
		if (radii[i] > max) {
			max = radii[i];
		}
	}
	Quadtree<index> quad(max+(1-max)/4);

	for (index i = 0; i < n; i++) {
		EXPECT_GE(angles[i], 0);
		EXPECT_LT(angles[i], 2*M_PI);
		EXPECT_GE(radii[i], 0);
		EXPECT_LT(radii[i], R);
		TRACE("Added (", angles[i], ",", radii[i], ")");
		quad.addContent(i, angles[i], radii[i]);
	}
	vector<index> all = quad.getElements();
	EXPECT_EQ(all.size(), n);
	for (index testindex = 0; testindex < 100; testindex++) {
		index comparison = Aux::Random::integer(n-1);
		Point<double> origin;
		Point<double> query = HyperbolicSpace::polarToCartesian(angles[comparison], radii[comparison]);
		DEBUG("Using ", comparison, " at (", angles[comparison], ",", radii[comparison], ") as query point");
		vector<index> closeToOne = quad.getCloseElements(HyperbolicSpace::polarToCartesian(angles[comparison], radii[comparison]), R);
		EXPECT_LE(closeToOne.size(), n);

		for (index i = 0; i < closeToOne.size(); i++) {
			//no corrupt indices
			ASSERT_LE(closeToOne[i], n);

			//close results should actually be close
			EXPECT_LE(HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], angles[closeToOne[i]], radii[closeToOne[i]]), R);
			for (index j = 0; j < i; j++) {
				/**
				 * results are unique
				 */
				EXPECT_NE(closeToOne[i], closeToOne[j]);
			}
	}

		for (index i = 0; i < n; i++) {
			if (HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], angles[i], radii[i]) < R) {
				bool found = false;
				QuadNode<index> responsibleNode = * getRoot(quad).getAppropriateLeaf(angles[i], radii[i]);

				/**
				TRACE("Getting lower bound for responsible node");
				double bound = responsibleNode.distanceLowerBound(angles[comparison], radii[comparison]);
				double actualDistance = HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], angles[i], radii[i]);
				EXPECT_GE(actualDistance, bound);
				EXPECT_TRUE(responsibleNode.responsible(angles[i], radii[i]));
	*/
				for (index j = 0; j < closeToOne.size(); j++) {
					if (closeToOne[j] == i) {
						found = true;
						break;
					}
				}
				EXPECT_TRUE(found) << "dist(" << i << "," << comparison << ") = "
						<< HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], angles[i], radii[i]) << " < " << R;
				if (!found) {
					DEBUG("angle: ", angles[i], ", radius: ", radii[i], ", leftAngle: ", responsibleNode.getLeftAngle(),
							", rightAngle: ", responsibleNode.getRightAngle(), ", minR: ", responsibleNode.getMinR(), ", maxR:", responsibleNode.getMaxR());
					DEBUG("dist(", comparison, ", leftMin)=", HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], responsibleNode.getLeftAngle(), responsibleNode.getMinR()));
					DEBUG("dist(", comparison, ", leftMax)=", HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], responsibleNode.getLeftAngle(), responsibleNode.getMaxR()));
					DEBUG("dist(", comparison, ", rightMin)=", HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], responsibleNode.getRightAngle(), responsibleNode.getMinR()));
					DEBUG("dist(", comparison, ", rightMax)=", HyperbolicSpace::getHyperbolicDistance(angles[comparison], radii[comparison], responsibleNode.getRightAngle(), responsibleNode.getMaxR()));
					DEBUG("drawsetup{", responsibleNode.getLeftAngle(), "}{", responsibleNode.getMaxR(), "}{", responsibleNode.getRightAngle(), "}{", responsibleNode.getMaxR(), "}{", R, "}");
					Point<double> witness = HyperbolicSpace::polarToCartesian(angles[i], radii[i]);
					Point<double> shadowImage = HyperbolicSpace::mirrorOnCircle(witness, origin, R);
					Point<double> circleCenter = HyperbolicSpace::circleCenter(query, witness, shadowImage);
					Point<double> upperLeft = HyperbolicSpace::polarToCartesian(responsibleNode.getLeftAngle(), responsibleNode.getMaxR());
					Point<double> upperRight = HyperbolicSpace::polarToCartesian(responsibleNode.getRightAngle(), responsibleNode.getMaxR());
					if (HyperbolicSpace::isBelowArc(query, upperLeft, upperRight, R)) {
						DEBUG("Witness point is below connecting arc.");
					} else {
						DEBUG("Witness point is above connecting arc.");
					}
					double centerangle, centerradius;
					double shadowangle, shadowradius;
					HyperbolicSpace::cartesianToPolar(circleCenter, centerangle, centerradius);
					HyperbolicSpace::cartesianToPolar(shadowImage, shadowangle, shadowradius);
					DEBUG("drawwitness{", angles[comparison], "}{", radii[comparison], "}{", angles[i], "}{", radii[i], "}{", shadowangle, "}{", shadowradius, "}{", centerangle, "}{", centerradius, "}{",
						circleCenter.distance(query), "}");
				}
			}
		}
	}
}

TEST_F(QuadTreeTest, testEuclideanCircle) {
	count n = 1000;
		double R = 1;
		vector<double> angles(n);
		vector<double> radii(n);
		HyperbolicSpace::fillPoints(&angles, &radii, 1, 1);
		double max = 0;
		for (index i = 0; i < n; i++) {
			if (radii[i] > max) {
				max = radii[i];
			}
		}
		Quadtree<index> quad(max+(1-max)/4);

		for (index i = 0; i < n; i++) {
			EXPECT_GE(angles[i], 0);
			EXPECT_LT(angles[i], 2*M_PI);
			EXPECT_GE(radii[i], 0);
			EXPECT_LT(radii[i], R);
			TRACE("Added (", angles[i], ",", radii[i], ")");
			quad.addContent(i, angles[i], radii[i]);
		}
		vector<index> all = quad.getElements();
		EXPECT_EQ(all.size(), n);
		QuadNode<index> root = getRoot(quad);
		for (index i = 0; i < 100; i++) {
			index comparison = Aux::Random::integer(n);
			Point<double> origin;
			Point<double> query = HyperbolicSpace::polarToCartesian(angles[comparison], radii[comparison]);
			double radius = Aux::Random::real(1);//this may overshoot the poincaré disc, this is intentional. I want to know what happens
			double minR = query.length() - radius;
			double maxR = query.length() + radius;
			double minPhi, maxPhi, phi_c, r_c, spread;
			if (minR < 0) {
				maxR = std::max(abs(minR), maxR);
				minR = 0;
				minPhi = 0;
				maxPhi = 2*M_PI;
			} else {
				spread = asin(radius / query.length());
				HyperbolicSpace::cartesianToPolar(query, phi_c, r_c);
				minPhi = phi_c - spread;
				maxPhi = phi_c + spread;
				/**
				 * what to do if they overlap the 2\pi line? Well, have to make two separate calls and collect
				 */
			}

			/**
			 * get Elements in Circle
			 */

			vector<index> circleDenizens;
			vector<Point<double> > positions;

			root.getElementsInEuclideanCircle(minPhi, maxPhi, minR, maxR, query, radius, circleDenizens, positions);
			if (minPhi < 0) {
				root.getElementsInEuclideanCircle(2*M_PI+minPhi, 2*M_PI, minR, maxR, query, radius, circleDenizens, positions);
			}
			if (maxPhi > 2*M_PI) {
				root.getElementsInEuclideanCircle(0, maxPhi - 2*M_PI, minR, maxR, query, radius, circleDenizens, positions);
			}

			for (index j = 0; j < n; j++) {
				Point<double> comp = HyperbolicSpace::polarToCartesian(angles[j], radii[j]);
				double dist = comp.distance(query);
				if (dist < radius) {
					bool found = false;
					for (index k = 0; k < circleDenizens.size(); k++) {
						if (circleDenizens[k] == j) {
							found = true;
						}
					}
					EXPECT_TRUE(found)<< "dist(" << j << "," << comparison << ") = "
							<< dist << " < " << radius;
					if (!found) {
						DEBUG("angle: ", angles[j], ", radius: ", radii[j]);
					}
				}
			}
		}
}

} /* namespace NetworKit */
