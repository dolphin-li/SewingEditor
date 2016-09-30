#include "GL\glew.h"
#include "SvgPath.h"
#include "SvgAttribute.h"
#include "SvgGroup.h"
namespace svg
{
#undef min
#undef max
	static GLenum lineJoinConverter(const SvgPath *path)
	{
		switch (path->m_pathStyle.line_join) {
		default:
			assert(!"bad line_join");
		case PathStyle::MITER_TRUNCATE_JOIN:
			return GL_MITER_TRUNCATE_NV;
		case PathStyle::MITER_REVERT_JOIN:
			return GL_MITER_REVERT_NV;
		case PathStyle::ROUND_JOIN:
			return GL_ROUND_NV;
		case PathStyle::BEVEL_JOIN:
			return GL_BEVEL_NV;
		case PathStyle::NONE_JOIN:
			return GL_NONE;
		}
	}

	static GLenum lineCapConverter(const SvgPath *path)
	{
		switch (path->m_pathStyle.line_cap) {
		default:
			assert(!"bad line_cap");
		case PathStyle::BUTT_CAP:
			return GL_FLAT;
		case PathStyle::ROUND_CAP:
			return GL_ROUND_NV;
		case PathStyle::SQUARE_CAP:
			return GL_SQUARE_NV;
		case PathStyle::TRIANGLE_CAP:
			return GL_TRIANGULAR_NV;
		}
	}

	SvgPath::SvgPath() :SvgAbstractObject()
	{
		m_gl_path_id = glGenPathsNV(1);
		m_boxColor = ldp::Float3(1, 0, 1);
		m_boxStrokeWidth = 2;
	}

	SvgPath::~SvgPath()
	{
		if (m_gl_path_id)
			glDeletePathsNV(m_gl_path_id, 1);
	}

	void SvgPath::render()
	{
		assert(m_gl_path_id);

		bool ancestorSelected = false;
		if (ancestorAfterRoot())
			ancestorSelected = ancestorAfterRoot()->isSelected();

		glColor3fv(attribute()->m_color.ptr());
		if (isHighlighted() || isSelected() || ancestorSelected)
			glColor3f(0, 0, 1);
		if (isSelected())
			renderBounds(false);

		if (m_invalid)
		{
			cacheNvPaths();
			m_invalid = false;
		}// end if invalid

		configNvParams();
		glStencilStrokePathNV(m_gl_path_id, 1, ~0);
		glCoverStrokePathNV(m_gl_path_id, GL_BOUNDING_BOX_NV);	
		renderSelection();
	}

	void SvgPath::renderId()
	{
		assert(m_gl_path_id);
		glColor4fv(color_from_index(m_id).ptr());

		if (m_invalid)
		{
			cacheNvPaths();
			m_invalid = false;
		}// end if invalid

		configNvParams();
		glStencilStrokePathNV(m_gl_path_id, 1, ~0);
		glCoverStrokePathNV(m_gl_path_id, GL_BOUNDING_BOX_NV);
	}

	void SvgPath::cacheNvPaths()
	{
		glPathCommandsNV(m_gl_path_id,
			GLsizei(m_cmds.size()), &m_cmds[0],
			GLsizei(m_coords.size()), GL_FLOAT, &m_coords[0]);
	}

	void SvgPath::configNvParams()
	{
		glPathParameteriNV(m_gl_path_id, GL_PATH_JOIN_STYLE_NV, lineJoinConverter(this));
		glPathParameteriNV(m_gl_path_id, GL_PATH_END_CAPS_NV, lineCapConverter(this));
		glPathParameterfNV(m_gl_path_id, GL_PATH_STROKE_WIDTH_NV, m_pathStyle.stroke_width);
		glPathParameterfNV(m_gl_path_id, GL_PATH_MITER_LIMIT_NV, m_pathStyle.miter_limit);
		if (m_pathStyle.dash_array.size())
		{
			glPathDashArrayNV(m_gl_path_id, GLsizei(m_pathStyle.dash_array.size()), &m_pathStyle.dash_array[0]);
			glPathParameteriNV(m_gl_path_id, GL_PATH_DASH_CAPS_NV, lineCapConverter(this));
			glPathParameterfNV(m_gl_path_id, GL_PATH_DASH_OFFSET_NV, m_pathStyle.dash_offset);
			glPathParameteriNV(m_gl_path_id, GL_PATH_DASH_OFFSET_RESET_NV, m_pathStyle.dash_phase);
		}
		else
		{
			glPathDashArrayNV(m_gl_path_id, 0, NULL);
		}
	}

	void SvgPath::renderSelection()
	{
		bool ancestorSelected = false;
		if (ancestorAfterRoot())
			ancestorSelected = ancestorAfterRoot()->isSelected();
		if (isHighlighted() || isSelected() || ancestorSelected)
		{
			float sz = m_pathStyle.stroke_width / 5;
			if (isHighlighted() && (isSelected() || ancestorSelected))
				sz *= 2;
			// render control points
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDisable(GL_STENCIL_TEST);
			glBegin(GL_LINES);
			for (int i = 0; i < (int)m_segmentPos.size() - 1; i++)
			{
				int bg = m_segmentPos[i];
				int ed = m_segmentPos[i + 1];
				for (int j = bg; j < ed - 1; j += 2)
				{
					ldp::Float2 c(m_coords[j], m_coords[j + 1]);
					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] - sz);

					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] - sz, c[1] + sz);

					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] + sz, c[1] - sz);

					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] - sz, c[1] + sz);
				}
			}
			glEnd();
			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);
			for (int i = 0; i < (int)m_segmentPos.size() - 1; i++)
			{
				int bg = m_segmentPos[i];
				int ed = m_segmentPos[i + 1];
				for (int j = bg; j < ed - 1; j += 2)
				{
					ldp::Float2 c(m_coords[j], m_coords[j + 1]);
					glVertex2f(c[0] - sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] - sz);
					glVertex2f(c[0] + sz, c[1] + sz);
					glVertex2f(c[0] - sz, c[1] + sz);
				}
			}
			glEnd();
			glPopAttrib();
		}
	}

	std::shared_ptr<SvgAbstractObject> SvgPath::splitToSegments()const
	{
		std::shared_ptr<SvgAbstractObject> group;
		if (m_cmds.size() == 0)
			return group;

		// we DONOT process closed path, such as a circle
		if (m_cmds.back() == GL_CLOSE_PATH_NV)
			return group;

		// find the position of 'M'
		std::vector<int> posOfM;
		for (size_t i = 0; i < m_cmds.size(); i++)
		{
			if (m_cmds[i] == GL_MOVE_TO_NV)
				posOfM.push_back(i);
		}
		posOfM.push_back(m_cmds.size());

		if (posOfM.size() <= 2)
			return group;

		group.reset(new SvgGroup);
		SvgGroup* groupPtr = (SvgGroup*)group.get();

		for (int i = 0; i < (int)posOfM.size() - 1; i++)
		{
			int posOfM_Begin = posOfM[i];
			int posOfM_End = posOfM[i + 1];
			SvgPath* subPath = new SvgPath();
			*subPath->attribute() = *attribute();
			subPath->setHighlighted(false);
			subPath->setSelected(m_selected);
			subPath->setParent(groupPtr);
			subPath->m_gl_fill_rull = m_gl_fill_rull;
			subPath->m_pathStyle = m_pathStyle;
			groupPtr->m_children.push_back(std::shared_ptr<SvgAbstractObject>(subPath));

			// cmds
			subPath->m_cmds.insert(subPath->m_cmds.end(), 
				m_cmds.begin() + posOfM_Begin, m_cmds.begin() + posOfM_End);

			// pos
			subPath->m_segmentPos.insert(subPath->m_segmentPos.end(),
				m_segmentPos.begin() + posOfM_Begin, m_segmentPos.begin() + posOfM_End);
			for (auto& v : subPath->m_segmentPos)
				v -= m_segmentPos[posOfM_Begin];

			// coords
			int cb = m_segmentPos[posOfM_Begin];
			int ce = m_coords.size();
			if (posOfM_End < m_segmentPos.size())
				ce = m_segmentPos[posOfM_End];
			subPath->m_coords.insert(subPath->m_coords.end(),
				m_coords.begin() + cb, m_coords.begin() + ce);

			// bounds
			subPath->updateBoundFromGeometry();
		}

		return group;
	}

#pragma region --bounds helper
	struct EndPointArc {
		ldp::Float2 p[2],
		radii;
		float x_axis_rotation;
		bool large_arc_flag,
			sweep_flag;
	};

	struct CenterPointArc {
		enum {
			BEHAVED,          // draw an arc
			DEGENERATE_LINE,  // draw a line connecting p0 and p1
			DEGENERATE_POINT  // don't draw anything (p0 == p1)
		} form;
		ldp::Float2 center;
		ldp::Float2 radii;
		ldp::Float2 p[2];
		float theta1;
		float delta_theta;
		float psi;

		template <typename T>
		static inline T radians(const  T& v)
		{
			return T(0.017453292) * v;
		}

		ldp::Double2 yx(ldp::Double2 v)
		{
			return ldp::Double2(v[1], v[0]);
		}

		// Equation F.6.5.4
		static double arc_angle_helper(ldp::Double2 u, ldp::Double2 v)
		{
			double cos_value = u.dot(v) / (u.length() * v.length()),
				clamped_cos_value = std::max(-1.0, std::min(1.0, cos_value));
			double theta_sign = double(u[0]*v[1] - u[1]*v[0] < 0 ? -1 : 1);
			double angle = theta_sign * acos(clamped_cos_value);
			return angle;
		}

		template <typename T>
		static bool illbehaved(T a)
		{
			return std::isnan(a) || std::isinf(a);
		}
		template <typename T>
		static bool illbehaved(ldp::ldp_basic_vec2<T> a)
		{
			return std::isnan(a[0]) || std::isinf(a[0])
				|| std::isnan(a[1]) || std::isinf(a[1]);
		}

		CenterPointArc(const EndPointArc &src)
		{
			// Handle these two easy/common degenerate cases first.
			if (src.p[0] == src.p[1]) {
				// "If the endpoints (x1, y1) and (x2, y2) are identical, then this
				// is equivalent to omitting the elliptical arc segment entirely."
				CenterPointArc::form = DEGENERATE_POINT;
				// other members left undefined
				return;
			}
			if (src.radii[0] == 0 || src.radii[1] == 0) {
				CenterPointArc::form = DEGENERATE_LINE;
				CenterPointArc::p[0] = src.p[0];
				CenterPointArc::p[1] = src.p[1];
				// other members left undefined
				return;
			}

			// Conversion from endpoint arc to center arc parameterization
			// See http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes
			const ldp::Double2 v1 = ldp::Double2(src.p[0]),  // v1.x = x1, v1.y = y1
				v2 = ldp::Double2(src.p[1]);  // v2.x = x2, v2.y = x2

			// Ensure radii are positive
			// Take the absolute value of rX and rY:
			ldp::Double2 r = ldp::Double2(std::abs(src.radii[0]),
				std::abs(src.radii[1]));  // r not const since could be adjusted if too small

			// Any nonzero value for either of the flags fA or fS is taken to mean the value 1
			const bool fA = src.large_arc_flag,
				fS = src.sweep_flag;

			// psi is taken mod 360 degrees.
			// [fine since sin and cos functions do this (better)]
			const double psi = radians(double(src.x_axis_rotation));

			// Step 1: Compute (x1', y1') according to the formula
			// cw = clockwise
			ldp::Mat2d cw_rotate_by_psi;
			cw_rotate_by_psi(0, 0) = cos(psi);
			cw_rotate_by_psi(1, 0) = sin(psi);
			cw_rotate_by_psi(0, 1) = -sin(psi);
			cw_rotate_by_psi(1, 1) = cos(psi);

			const ldp::Double2 half_v_diff = (v1 - v2) / 2,
				v1p = cw_rotate_by_psi * half_v_diff;

			{ // BEGIN F.6.6 Correction of out-of-range radii

				// Step 1: Ensure radii are non-zero
				// If rX = 0 or rY = 0, then treat this as a straight line
				// from (x1, y1) to (x2, y2) and stop.
				// [already earlier]

				// Otherwise,
				// Step 2: Ensure radii are positive
				// Take the absolute value of rX and rY:
				// [already done]

				// Step 3: Ensure radii are large enough
				// Using the primed coordinate values of equation (F.6.5.1), compute
				const double Lambda = (v1p / r).dot(v1p / r);

				// If the result of the above equation is less than or equal to 1,
				// then no further change need be made to rX and rY.  If the result
				// of the above equation is greater than 1, then make the replacements
				if (Lambda > 1) {
					r = sqrt(Lambda)*r;
				}
				// Step 4: Proceed with computations

				// Proceed with the remaining elliptical arc computations, such as
				// those in section F.6.5.  Note: As a consequence of the radii
				// corrections in this section, equation (F.6.5.2) for the center
				// of the ellipse always has at least one solution (i.e. the
				// radicand is never negative).  In the case that the radii are
				// scaled up using equation (F.6.6.3), the radicand of (F.6.5.2)
				// is zero and there is exactly one solution for the center of the
				// ellipse.
			} // END F.6.6 Correction of out-of-range radii

			// Step 2: Compute (cX', cY') according to the [re-arranged] formula
			// Rearrange sqrt((rx^2*ry^2 - rx^2*y1p^2 - ry^2*x1p^2)/
			//                (rx^2*y1p^2 + ry^2*x1p^2))
			// to be sqrt(rx^2*ry^2)/(rx^2*y1p^2 + ry^2*x1p^2))-1
			// because sqrt((- rx^2*y1p^2 - ry^2*x1p^2)/
			//              (rx^2*y1p^2 + ry^2*x1p^2))
			// equals -1
			double numer = r[0]*r[0] * r[1]*r[1],
				denom = (r*r).dot(yx(v1p*v1p));
			double radicand = numer / denom - 1;
			// Add max to make sure numer/denom-1 can't be negative
			double k = sqrt(std::max(0., radicand));
			// where the + sign is chosen if fA notequal fS
			// and the - sign is chosen if fA equal fS
			if (fA == fS) {
				k = -k;
			}
			ldp::Double2 cp = k*(r*yx(v1p / r));
			cp[1] = -cp[1];

			// Step 3: Compute (cX, cY) from (cX', cY')
			// ccw = counterclockwise
			const ldp::Mat2d ccw_rotate_by_psi = cw_rotate_by_psi.trans();
			const ldp::Double2 average_v = (v1 + v2) / 2,
				c = ccw_rotate_by_psi * cp + average_v;

			// Step 4: Compute theta1 and delta-theta

			// This angle function can be used to express theta1 and delta-theta as follows
			double theta1 = arc_angle_helper(ldp::Double2(1, 0), (v1p - cp) / r),
				delta_theta = arc_angle_helper((v1p - cp) / r, (0.-v1p - cp) / r);
			// where theta1 is fixed in the range -360 degrees < delta-theta < 360
			// degrees such that:
			// if fS equal 0, then delta-theta < 0,
			// else if fS equal 1, then delta-theta > 0.
			//
			// In other words, if fS = 0 and the right side of (F.6.5.6) is > 0,
			// then subtract 360 degrees, whereas if fS = 1 and the right side of
			// (F.6.5.6) is < 0, then add 360 degrees.  In all other cases leave it as is.
			const double pi = M_PI;
			if (!fS && delta_theta > 0) {
				delta_theta = delta_theta - 2 * pi;
			}
			else if (fS && delta_theta < 0) {
				delta_theta = delta_theta + 2 * pi;
			}

			// Weird stuff could happen due to division by zero or
			// overflow; treat any such situation as a degenerate line.
			if (illbehaved(c) || illbehaved(r) ||
				illbehaved(theta1) || illbehaved(delta_theta) || illbehaved(psi)) {
				CenterPointArc::form = DEGENERATE_LINE;
				CenterPointArc::p[0] = src.p[0];
				CenterPointArc::p[1] = src.p[1];
				// other members left undefined
				return;
			}

			// Assign back to the member variables 
			// (scoping avoids ambiguity because several locals are declared
			// double precision with identical names to the member variables)
			CenterPointArc::form = BEHAVED;
			CenterPointArc::center = ldp::Float2(c);
			CenterPointArc::radii = ldp::Float2(r);
			CenterPointArc::p[0] = src.p[0];
			CenterPointArc::p[1] = src.p[1];
			CenterPointArc::theta1 = float(theta1);
			CenterPointArc::delta_theta = float(delta_theta);
			CenterPointArc::psi = float(psi);

			// Early return possible for degenerate cases!
		}


		CenterPointArc() {};

		// Evaluate a point on the arc at a given theta.
		ldp::Float2 eval(double theta)
		{
			ldp::Double2 result;
			double dpsi = psi,
				cos_psi = cos(dpsi),
				sin_psi = sin(dpsi),
				cos_theta = cos(theta),
				sin_theta = sin(theta);
			result[0] = cos_psi*radii[0]*cos_theta - sin_psi*radii[1]*sin_theta;
			result[1] = sin_psi*radii[0]*cos_theta + cos_psi*radii[1]*sin_theta;
			result += center;
			return ldp::Float2(result);
		}
	};

	struct GetBoundsPathSegmentProcessor 
	{
		typedef ldp::Float4 float4;
		typedef ldp::Float2 float2;
		typedef ldp::Double2 double2;
		float4 bbox;

		template <typename REAL>
		int quadratic(REAL b, REAL c, REAL rts[2])
		{
			int solutions = 0;
			rts[0] = 0;
			rts[1] = 0;

			REAL discriminant = b*b - 4 * c;
			if (b == 0) {
				if (c == 0) {
					solutions = 2;
				}
				else {
					if (c < 0) {
						solutions = 2;
						rts[0] = sqrt(-c);
						rts[1] = -rts[0];
					}
					else {
						solutions = 0;
					}
				}
			}
			else if (c == 0) {
				solutions = 2;
				rts[0] = -b;
			}
			else if (discriminant >= 0) {
				solutions = 2;
				REAL discriminant_sqrt = sqrt(discriminant);
				if (b > 0) {
					rts[0] = (-b - discriminant_sqrt)*(1 / REAL(2));
				}
				else {
					rts[0] = (-b + discriminant_sqrt)*(1 / REAL(2));
				}
				if (rts[0] == 0) {
					rts[1] = -b;
				}
				else {
					rts[1] = c / rts[0];
				}
			}
			return solutions;
		}

		GetBoundsPathSegmentProcessor()
			: bbox(FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX)
		{}

		void addPointToBounds(const float2 &p) {
			bbox[0] = std::min(bbox[0], p[0]);
			bbox[1] = std::max(bbox[1], p[0]);
			bbox[2] = std::min(bbox[2], p[1]);
			bbox[3] = std::max(bbox[3], p[1]);
		}
		void addXToBounds(const float x) {
			bbox[0] = std::min(bbox[0], x);
			bbox[1] = std::max(bbox[1], x);
		}
		void addYToBounds(const float y) {
			bbox[2] = std::min(bbox[2], y);
			bbox[3] = std::max(bbox[3], y);
		}
		void addComponentToBounds(int c, const float v) {
			c *= 2;
			bbox[c + 0] = std::min(bbox[c + 0], v);
			bbox[c + 1] = std::max(bbox[c + 1], v);
		}

		void lineTo(const float2 plist[2]) {
			addPointToBounds(plist[0]);
			addPointToBounds(plist[1]);
		}
		void quadraticCurveTo(const float2 plist[3]) {
			const double2 P0 = double2(plist[0]),
				P1 = double2(plist[1]),
				P2 = double2(plist[2]);

			// Quadratic Bezier parametric equation:
			// > Qeq:=P0*(1-t)^2+2*P1*(1-t)*t+P2*t^2;
			// One-half derivative of quadratic bezier parametric equation:
			// > Qteq:=diff(Qeq/2,t);
			//   Qteq:=-P0*(1-t)-P1*t+P1*(1-t)+P2*t
			// Collect coefficients of t terms:
			// > collect(Qteq/2,t);
			//   (P0-2*P1+P2)*t-P0+P1
			double2 a = P0 - 2.* P1 + P2,
				b = P1 - P0;

			// For X (j=0) and Y (j=1) components...
			for (int j = 0; j<2; j++) {
				if (a[j] == 0) {
					// Constant equation.  End-points will suffice as bounds.
				}
				else {
					// Linear equation.
					double t = -b[j] / a[j];
					// Is t in parametric [0,1] range of the segment?
					if (t > 0 && t < 1) {
						double v = (a[j] * t + 2 * b[j])*t + P0[j];
						addComponentToBounds(j, float(v));
					}
				}
			}

			addPointToBounds(plist[0]);
			addPointToBounds(plist[2]);
		}
		void cubicCurveTo(const float2 plist[4])
		{
			// Cubic bezier segment control points:
			const double2 P0 = double2(plist[0]),
				P1 = double2(plist[1]),
				P2 = double2(plist[2]),
				P3 = double2(plist[3]);

			// Cubic Bezier parametric equation:
			// > Ceq:=P0*(1-t)^3+3*P1*(1-t)^2*t+3*P2*t^2*(1-t)+P3*t^3;
			// One-third derivative of cubic bezier parametric equation:
			// > Cteq:=diff(Ceq/3,t);
			//   Cteq := -P0*(1-t)^2-2*P1*(1-t)*t+P1*(1-t)^2+2*P2*t*(1-t)-P2*t^2+P3*t^2
			// Collect coefficients of t terms:
			// > collect(Cteq/3,t);
			//   (P3-3*P2+3*P1-P0)*t^2+(-4*P1+2*P2+2*P0)*t+P1-P0
			const double2 a = P3 - 3. * P2 + 3. * P1 - P0,
				b = 2. * P2 - 4. * P1 + 2. * P0,
				c = P1 - P0;

			// For X (j=0) and Y (j=1) components...
			for (int j = 0; j<2; j++) {
				// Solve for "t=0" for Cteq
				if (a[j] == 0) {
					// Not quadratic.
					if (b[j] == 0) {
						// Constant equation.  End-points will suffice as bounds.
					}
					else {
						// Is linear equation.
						const double t = -c[j] / b[j];
						// Is t in parametric [0,1] range of the segment?
						if (t > 0 && t < 1) {
							// Form original cubic equation in Horner form and evaluate:
							const double v = P0[j] + (-3 * P0[j] + 3 * P1[j] + (3 * P0[j] + 3 * P2[j] - 6 * P1[j] + (3 * P1[j] - P0[j] - 3 * P2[j] + P3[j])*t)*t)*t;
							addComponentToBounds(j, float(v));
						}
					}
				}
				else {
					// Need the quadratic equation.
					double t_array[2];
					const int solutions = quadratic(b[j] / a[j], c[j] / a[j], t_array);
					// For each quadratic equation solution...
					for (int i = 0; i<solutions; i++) {
						const double t = t_array[i];
						// Is t in parametric [0,1] range of the segment?
						if (t > 0 && t < 1) {
							// Form original cubic equation in Horner form and evaluate:
							const double v = P0[j] + (-3 * P0[j] + 3 * P1[j] + (3 * P0[j] + 3 * P2[j] - 6 * P1[j] + (3 * P1[j] - P0[j] - 3 * P2[j] + P3[j])*t)*t)*t;
							addComponentToBounds(j, float(v));
						}
					}
				}
			}

			// Add initial and terminal points of cubic Bezier segment.
			addPointToBounds(plist[0]);
			addPointToBounds(plist[3]);
		}
		inline double wrapAngle(double theta) {
			if (std::isfinite(theta) && fabs(theta) < 4 * 2 * M_PI) {
				// XXX fmod is slow, but it may already to this optimization
				// we may want to do a perf test and see if this is worth it ...
				while (theta >= 2 * M_PI) {
					theta -= 2 * M_PI;
				}
				while (theta < 0) {
					theta += 2 * M_PI;
				}
			}
			else {
				theta = fmod(theta, 2 * M_PI);
			}

			return theta;
		}
		void arcTo(const EndPointArc &arc) {
			// Convert to a center point arc to be able to render the arc center.
			CenterPointArc center_point_arc(arc);

			// Add the arc's two end points to the bounding box.
			addPointToBounds(arc.p[0]);
			addPointToBounds(arc.p[1]);

			if (center_point_arc.form == CenterPointArc::BEHAVED) {
				double tan_psi = tan(center_point_arc.psi),
					rx = center_point_arc.radii[0],
					ry = center_point_arc.radii[1],
					theta1 = wrapAngle(center_point_arc.theta1),
					theta2 = wrapAngle(theta1 + center_point_arc.delta_theta);

				if (center_point_arc.delta_theta < 0) {
					double tmp = theta1;
					theta1 = theta2;
					theta2 = tmp;
				}

				// Use the partial elliptical arc segment center point parametric form...
				//
				// Solve for where gradient is zero in the interval [theta1,theta2].
				//
				// Ax:=cos(phi)*rx*cos(theta) - sin(phi)*ry*sin(theta) + cx;
				// diff(Ax,theta);
				//   -cos(phi)*rx*sin(theta)-sin(phi)*ry*cos(theta)
				// Ay:=sin(phi)*rx*cos(theta) + cos(phi)*ry*sin(theta) + cy;
				// diff(Ay,theta);
				//   -sin(phi)*rx*sin(theta)+cos(phi)*ry*cos(theta)
				// solve(diff(Ax,theta)=0,theta);
				//   -arctan(tan(phi)*ry/rx)
				// solve(diff(Ay,theta)=0,theta);
				//   arctan(ry/(tan(phi)*rx))
				double theta_x = wrapAngle(-atan2(tan_psi*ry, rx)),
					theta_y = wrapAngle(atan2(ry, tan_psi*rx));

				// Here we have two scenarios: theta1 < theta2, and theta1 > theta2 
				// (if they're equal the ellipse is degenerate). Conceptually, an angle is
				// within the partial arc if it falls within the set of angles passed over by
				// moving from theta1 to theta2 in the direction of positive angles.

				// This can be accoomplish by saying
				// if (theta1 > theta2)
				//    on_path = theta1 <= angle <= theta2
				// else
				//    on_path = angle >= theta1 && angle <= theta2

				// If we swap theta1 and theta2 when theta2 > theta1, then a simple
				// if ((theta1 < theta_x && theta_x < theta2) == not_swapped)
				// can also do the job because there's no need to worry about </<= boundaries -
				// we already called addPointToBounds() before for the points at theta1 and theta2

				bool not_swapped = true;
				if (theta1 > theta2) {
					not_swapped = false;
					double tmp = theta1;
					theta1 = theta2;
					theta2 = tmp;
				}

				// Is theta_x in [theta1,theta2] range?
				if ((theta1 < theta_x && theta_x < theta2) == not_swapped) {
					float2 v = center_point_arc.eval(theta_x);
					addXToBounds(v[0]);
				}
				// Is theta_y in [theta1,theta2] range?
				if ((theta1 < theta_y && theta_y < theta2) == not_swapped) {
					float2 v = center_point_arc.eval(theta_y);
					addYToBounds(v[1]);
				}
				theta_x = wrapAngle(theta_x + M_PI);
				theta_y = wrapAngle(theta_y + M_PI);
				// Is theta_x in [theta1,theta2] range?
				if ((theta1 < theta_x && theta_x < theta2) == not_swapped) {
					float2 v = center_point_arc.eval(theta_x);
					addXToBounds(v[0]);
				}
				// Is theta_y in [theta1,theta2] range?
				if ((theta1 < theta_y && theta_y < theta2) == not_swapped) {
					float2 v = center_point_arc.eval(theta_y);
					addYToBounds(v[1]);
				}
			}
			else {
				// In degenerate cases, end points are enough.
				switch (center_point_arc.form) {
				case CenterPointArc::DEGENERATE_LINE:
				case CenterPointArc::DEGENERATE_POINT:
					// Do nothing.
					break;
				default:
					assert(!"bogus CenterPointArc form");
					break;
				}
			}
		}
	};
#pragma endregion

	void SvgPath::updateBoundFromGeometry()
	{
		GetBoundsPathSegmentProcessor processor;
		ldp::Float2 last_p;
		ldp::Float2 p[4];
		EndPointArc arc;
		for (int i_pos = 0; i_pos < m_segmentPos.size(); i_pos++)
		{
			int posBegin = m_segmentPos[i_pos];
			int posEnd = m_coords.size();
			if(i_pos + 1 < m_segmentPos.size())
				posEnd = m_segmentPos[i_pos + 1];
			switch (m_cmds[i_pos])
			{
			case GL_MOVE_TO_NV:
				last_p = ldp::Float2(m_coords[posBegin], m_coords[posBegin+1]);
				break;
			case GL_LINE_TO_NV:
				p[0] = last_p;
				p[1] = ldp::Float2(m_coords[posBegin], m_coords[posBegin + 1]);
				processor.lineTo(p);
				last_p = p[1];
				break;
			case GL_QUADRATIC_CURVE_TO_NV:
				p[0] = last_p;
				p[1] = ldp::Float2(m_coords[posBegin], m_coords[posBegin + 1]);
				p[2] = ldp::Float2(m_coords[posBegin + 2], m_coords[posBegin + 3]);
				processor.quadraticCurveTo(p);
				last_p = p[2];
				break;
			case GL_CUBIC_CURVE_TO_NV:
				p[0] = last_p;
				p[1] = ldp::Float2(m_coords[posBegin], m_coords[posBegin + 1]);
				p[2] = ldp::Float2(m_coords[posBegin + 2], m_coords[posBegin + 3]);
				p[3] = ldp::Float2(m_coords[posBegin + 4], m_coords[posBegin + 5]);
				processor.cubicCurveTo(p);
				last_p = p[3];
				break;
			case GL_LARGE_CCW_ARC_TO_NV:
				arc.sweep_flag = true;
				arc.large_arc_flag = true;
				arc.radii = ldp::Float2(m_coords[posBegin], m_coords[posBegin + 1]);
				arc.x_axis_rotation = m_coords[posBegin + 2];
				arc.p[0] = last_p;
				arc.p[1] = ldp::Float2(m_coords[posBegin + 3], m_coords[posBegin + 4]);
				last_p = arc.p[1];
				processor.arcTo(arc);
				break;
			case GL_LARGE_CW_ARC_TO_NV:
				arc.sweep_flag = false;
				arc.large_arc_flag = true;
				arc.radii = ldp::Float2(m_coords[posBegin], m_coords[posBegin + 1]);
				arc.x_axis_rotation = m_coords[posBegin + 2];
				arc.p[0] = last_p;
				arc.p[1] = ldp::Float2(m_coords[posBegin + 3], m_coords[posBegin + 4]);
				last_p = arc.p[1];
				processor.arcTo(arc);
				break;
			case GL_SMALL_CCW_ARC_TO_NV:
				arc.sweep_flag = true;
				arc.large_arc_flag = false;
				arc.radii = ldp::Float2(m_coords[posBegin], m_coords[posBegin + 1]);
				arc.x_axis_rotation = m_coords[posBegin + 2];
				arc.p[0] = last_p;
				arc.p[1] = ldp::Float2(m_coords[posBegin + 3], m_coords[posBegin + 4]);
				last_p = arc.p[1];
				processor.arcTo(arc);
				break;
			case GL_SMALL_CW_ARC_TO_NV:
				arc.sweep_flag = false;
				arc.large_arc_flag = false;
				arc.radii = ldp::Float2(m_coords[posBegin], m_coords[posBegin + 1]);
				arc.x_axis_rotation = m_coords[posBegin + 2];
				arc.p[0] = last_p;
				arc.p[1] = ldp::Float2(m_coords[posBegin + 3], m_coords[posBegin + 4]);
				last_p = arc.p[1];
				processor.arcTo(arc);
				break;
			default:
				break;
			}
		}

		m_bbox = processor.bbox;
		m_bbox[0] -= m_pathStyle.stroke_width;
		m_bbox[1] += m_pathStyle.stroke_width;
		m_bbox[2] -= m_pathStyle.stroke_width;
		m_bbox[3] += m_pathStyle.stroke_width;
	}
}