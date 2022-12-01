using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Weapon : MonoBehaviour
{
    [SerializeField] Animator m_animator;
    [SerializeField]CapsuleCollider m_weapon;
    bool m_canChainAttack = false;
    // Start is called before the first frame update
    void Start()
    {
        m_weapon.enabled = false;
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            m_animator.SetBool("Attacking", true);
            if(m_canChainAttack)
            {
                m_animator.SetBool("FollowUp", true);
            }
        }
    }

    public void EnableAttacking()
    {
        m_weapon.enabled = true;
    }
    public void DisableAttacking()
    {
        m_weapon.enabled = false;
        m_animator.SetBool("Attacking", false);
    }

    public void EnableFollowUp()
    {
        m_canChainAttack = true;
    }

    public void DisableFollowUp()
    {
        m_canChainAttack = false;
        m_animator.SetBool("FollowUp", false);
    }
}
