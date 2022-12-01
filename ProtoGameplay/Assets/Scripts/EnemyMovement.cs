using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyMovement : MonoBehaviour
{
    [SerializeField] float m_detectionRadius;
    [SerializeField] float m_attackRadius;
    [SerializeField] Transform m_target;
    [SerializeField] Animator m_animator;
    Rigidbody m_rb;
    Vector3 m_vectorToTarget;
    float m_maxVelocity = 4f;
    bool m_isAttacking = false;
    bool m_isMoving = false;
    // Start is called before the first frame update
    void Start()
    {
        m_rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        if (m_target != null)
        {
            m_vectorToTarget = m_target.position - transform.position;
            if (m_vectorToTarget.magnitude < m_attackRadius && !m_isAttacking)
            {
                m_animator.SetBool("Attacking", true);
                m_isAttacking = true;
                m_maxVelocity = 3f;
            }
            else if (m_vectorToTarget.magnitude > m_attackRadius && m_isAttacking)
            {
                m_animator.SetBool("Attacking", false);
                m_isAttacking = false;
                m_maxVelocity = 4f;
            }

            if (m_vectorToTarget.magnitude < m_detectionRadius)
            {
                if (!m_isMoving)
                {
                    m_animator.SetBool("Moving", true);
                    m_isMoving = true;
                }
                m_rb.velocity = m_vectorToTarget.normalized * m_maxVelocity;
                transform.LookAt(m_target);
            }
            else if (m_isMoving)
            {
                m_animator.SetBool("Moving", false);
                m_isMoving = false;
            }
        }
        else
        {
            if (m_isAttacking)
            {
                m_isAttacking = false;
                m_animator.SetBool("Attacking", false);
            }
            if (m_isMoving)
            {
                m_isAttacking = false;
                m_animator.SetBool("Moving", false);
            }
        }
    }
}
