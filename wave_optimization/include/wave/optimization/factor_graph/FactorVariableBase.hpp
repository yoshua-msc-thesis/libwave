/**
 * @file
 * @ingroup optimization
 */

#ifndef WAVE_OPTIMIZATION_FACTOR_GRAPH_FACTORVARIABLEBASE_HPP
#define WAVE_OPTIMIZATION_FACTOR_GRAPH_FACTORVARIABLEBASE_HPP

namespace wave {
/** @addtogroup optimization
 *  @{ */

/**  Abstract base for all factor graph variables. */
class FactorVariableBase {
 public:
    virtual ~FactorVariableBase() = default;

    /** Return the number of scalar values in the variable. */
    virtual int size() const noexcept = 0;

    /** Return a raw pointer to the start of the internal storage. */
    virtual const double *data() const noexcept = 0;
    virtual double *data() noexcept = 0;

    /** Marks as constant during optimization
     * @todo replace with unary factors for priors
     */
    void setFixed(bool c) noexcept {
        this->fixed = c;
    }

    /** Whether this has been marked constant during optimization
     * @todo replace with unary factors for priors
     */
    bool isFixed() const noexcept {
        return this->fixed;
    }

    /** Print representation of the object for debugging.
     * Called by `operator<<`. */
    virtual void print(std::ostream &os) const = 0;

 private:
    bool fixed = false;
};

inline std::ostream &operator<<(std::ostream &os, const FactorVariableBase &v) {
    v.print(os);
    return os;
}

/** @} group optimization */
}  // namespace wave

#endif  // WAVE_OPTIMIZATION_FACTOR_GRAPH_FACTORVARIABLEBASE_HPP
