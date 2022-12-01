using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyAttack : MonoBehaviour
{

    EnemyMovement m_movement;

    private void Start()
    {
        m_movement = GetComponentInParent<EnemyMovement>();
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Player")
        {
            other.GetComponent<Movement>().Hit();
        }
    }
}
