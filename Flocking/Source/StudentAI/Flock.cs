using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FullSailAFI.SteeringBehaviors.Core;

namespace FullSailAFI.SteeringBehaviors.StudentAI
{
    public class Flock
    {
        public float AlignmentStrength { get; set; }
        public float CohesionStrength { get; set; }
        public float SeparationStrength { get; set; }
        public List<MovingObject> Boids { get; protected set; }
        public Vector3 AveragePosition { get; set; }
        protected Vector3 AverageForward { get; set; }
        public float FlockRadius { get; set; }

        #region Constructors
        public Flock()
        {
        }
        #endregion

        #region TODO Suggested helper methods

        private void CalculateAverages()
        {
            AverageForward = Vector3.Empty;
            AveragePosition = Vector3.Empty;

            foreach (MovingObject b in Boids) 
            {
                AveragePosition += b.Position;
                AverageForward += b.Velocity;
            }

            AveragePosition /= Boids.Count;
            AverageForward /= Boids.Count;

            return;
        }

        private Vector3 CalculateAlignmentAcceleration(MovingObject boid)
        {
            Vector3 vec_Alignment = AverageForward / boid.MaxSpeed; 

            if (vec_Alignment.LengthSquared > 1.0f)
            {
                vec_Alignment.Normalize();
            }

            return vec_Alignment * AlignmentStrength;
        }

        private Vector3 CalculateCohesionAcceleration(MovingObject boid)
        {
            Vector3 vec_Cohesion = AveragePosition - boid.Position;
            float dist = vec_Cohesion.Length;
            vec_Cohesion.Normalize();

            if (dist < FlockRadius)
            {
                vec_Cohesion *= dist / FlockRadius;
            }

            return vec_Cohesion * CohesionStrength;
        }

        private Vector3 CalculateSeparationAcceleration(MovingObject boid)
        {
            Vector3 vec_Seperation = Vector3.Empty;

            foreach (MovingObject otherBoid in Boids)
            {
                if (boid != otherBoid)
                {
                    Vector3 vec_Temp = boid.Position - otherBoid.Position;
                    float dist = vec_Temp.Length;
                    float dist_Safe = boid.SafeRadius + otherBoid.SafeRadius;

                    if (dist < dist_Safe)
                    {
                        vec_Temp.Normalize();
                        vec_Temp *= (dist_Safe - dist) / dist_Safe;
                        vec_Seperation += vec_Temp;
                    }
                }

                if (vec_Seperation.LengthSquared > 1.0f)
                {
                    vec_Seperation.Normalize();
                }
            }

            return vec_Seperation * SeparationStrength;
        }

        #endregion

        #region TODO

        public virtual void Update(float deltaTime)
        {
            CalculateAverages();

            foreach (MovingObject b in Boids)
            {
                float multiplier = b.MaxSpeed;

                Vector3 vec_Acceleration = CalculateAlignmentAcceleration(b);
                vec_Acceleration += CalculateCohesionAcceleration(b);
                vec_Acceleration += CalculateSeparationAcceleration(b);
                vec_Acceleration *= (multiplier * deltaTime);

                b.Velocity += vec_Acceleration;
                if (b.Velocity.Length > b.MaxSpeed)
                {
                    b.Velocity.Normalize();
                    b.Velocity *= b.MaxSpeed;
                }

                b.Update(deltaTime);
            }
        }
        #endregion
    }
}
